*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 100          /* tamanho máximo das strings (incluindo o '\0') */
#define DISPONIVEL 0     /* status do livro: 0 = disponível */
#define EMPRESTADO 1     /* status do livro: 1 = emprestado */

/* ------------------------------------------------------------------ */
/* ESTRUTURAS DE DADOS                                                */
/* ------------------------------------------------------------------ */

/* Nó da lista de usuários */
typedef struct Usuario {
    char email[TAM];
    char nome[TAM];
    struct Usuario *prox;      /* ponteiro para o próximo usuário */
} Usuario;

/* Nó da lista de livros */
typedef struct Livro {
    int id;   (sequencial) 
    char titulo[TAM];
    char autor[TAM];
    int ano;
    int status;                /* DISPONIVEL ou EMPRESTADO */
    char emailEmprestimo[TAM]; /* email de quem pegou; "" se disponível */
    struct Livro *prox;        /* ponteiro para o próximo livro */
} Livro;

/* Nó da árvore binária auxiliar (EXTRA): guarda apenas os IDs dos livros.
   Regra da árvore de busca: IDs menores vão para a esquerda, maiores para a direita. */
typedef struct NoArvore {
    int id;
    struct NoArvore *esq;      /* subárvore com IDs menores */
    struct NoArvore *dir;      /* subárvore com IDs maiores */
} NoArvore;

/* ------------------------------------------------------------------ */
/* FUNÇÕES AUXILIARES DE ENTRADA                                      */
/* ------------------------------------------------------------------ */

/* Lê um inteiro com segurança: se o usuário digitar letras, pede de novo. */
int lerInt(void) {
    int valor;
    while (scanf("%d", &valor) != 1) {
        if (feof(stdin)) exit(0);          /* evita loop infinito se a entrada acabar */
        while (getchar() != '\n');         /* descarta o lixo do buffer */
        printf("Entrada invalida. Digite um numero: ");
    }
    return valor;
}

/* Lê uma string sem espaços, limitando o tamanho para não estourar o vetor. */
void lerString(const char *mensagem, char *destino) {
    printf("%s", mensagem);
    scanf("%99s", destino);   /* 99 = TAM - 1 (reserva espaço para o '\0') */
}

/* ------------------------------------------------------------------ */
/* FUNÇÕES DE BUSCA                                                   */
/* ------------------------------------------------------------------ */

/* ----- Árvore binária de IDs (EXTRA) -----
   Obs.: quando um livro é excluído, seu ID continua na árvore. Assim, um código
   já usado nunca é reaproveitado por outro livro. */

/* Insere um ID na árvore (recursivo). Retorna a raiz da (sub)árvore. */
NoArvore *inserirNaArvore(NoArvore *raiz, int id) {
    if (raiz == NULL) {                       /* achou o lugar vazio: cria o nó aqui */
        NoArvore *no = (NoArvore *) malloc(sizeof(NoArvore));
        if (no == NULL) {
            printf("Erro: memoria insuficiente.\n");
            return NULL;
        }
        no->id = id;
        no->esq = NULL;
        no->dir = NULL;
        return no;
    }
    if (id < raiz->id) raiz->esq = inserirNaArvore(raiz->esq, id);
    else if (id > raiz->id) raiz->dir = inserirNaArvore(raiz->dir, id);
    return raiz;                              /* se id == raiz->id, já existe: não insere */
}

/* Retorna 1 se o ID existe na árvore, 0 caso contrário (busca iterativa). */
int existeNaArvore(NoArvore *raiz, int id) {
    while (raiz != NULL) {
        if (id == raiz->id) return 1;
        raiz = (id < raiz->id) ? raiz->esq : raiz->dir;   /* desce para o lado certo */
    }
    return 0;
}

/* Libera a árvore inteira. Pós-ordem: libera os filhos antes de liberar o pai. */
void liberarArvore(NoArvore *raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

/* ----- Busca de livros e usuários ----- */

/* Procura um livro pelo ID. Retorna o ponteiro ou NULL se não existir. */
Livro *buscarLivroPorId(Livro *lista, int id) {
    while (lista != NULL) {
        if (lista->id == id) return lista;
        if (lista->id > id) return NULL;   /* lista ordenada: já passou, não existe */
        lista = lista->prox;
    }
    return NULL;
}

/* Procura um usuário pelo email. Retorna o ponteiro ou NULL. */
Usuario *buscarUsuarioPorEmail(Usuario *lista, const char *email) {
    while (lista != NULL) {
        if (strcmp(lista->email, email) == 0) return lista;
        lista = lista->prox;
    }
    return NULL;
}

/* Mostra os dados de um livro na tela. */
void imprimirLivro(Livro *l) {
    printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Status: %s",
           l->id, l->titulo, l->autor, l->ano,
           l->status == DISPONIVEL ? "Disponivel" : "Emprestado");
    if (l->status == EMPRESTADO)
        printf(" (com: %s)", l->emailEmprestimo);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/* CADASTRO                                                           */
/* ------------------------------------------------------------------ */

/* Insere o livro na posição correta para manter a lista ordenada por ID. */
void inserirLivroOrdenado(Livro **lista, Livro *novo) {
    /* Caso 1: lista vazia ou novo vem antes do primeiro */
    if (*lista == NULL || novo->id < (*lista)->id) {
        novo->prox = *lista;
        *lista = novo;
        return;
    }
    /* Caso 2: anda até achar o último nó com ID menor que o novo */
    Livro *atual = *lista;
    while (atual->prox != NULL && atual->prox->id < novo->id)
        atual = atual->prox;
    novo->prox = atual->prox;
    atual->prox = novo;
}

/* Cadastra um livro. O ID é gerado automaticamente (sequencial). A árvore de IDs
   confirma, de forma rápida, que o código ainda não foi usado. */
void cadastrarLivro(Livro **lista, NoArvore **arvore, int *proximoId) {
    int id = *proximoId;
    if (existeNaArvore(*arvore, id)) {   /* verificação de duplicidade pela árvore */
        printf("Erro: o codigo %d ja existe.\n", id);
        return;
    }

    Livro *novo = (Livro *) malloc(sizeof(Livro));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }

    novo->id = id;
    (*proximoId)++;                     /* só avança o contador se o cadastro seguiu */
    lerString("Titulo: ", novo->titulo);
    lerString("Autor: ", novo->autor);
    printf("Ano de publicacao: ");
    novo->ano = lerInt();
    novo->status = DISPONIVEL;          /* todo livro novo começa disponível */
    novo->emailEmprestimo[0] = '\0';    /* string vazia = ninguém com o livro */
    novo->prox = NULL;

    inserirLivroOrdenado(lista, novo);
    *arvore = inserirNaArvore(*arvore, novo->id);   /* registra o ID na árvore */
    printf("Livro cadastrado com sucesso! Codigo (ID): %d\n", novo->id);
}

/* Cadastra um usuário, recusando email repetido. */
void cadastrarUsuario(Usuario **lista) {
    char email[TAM];
    lerString("Email: ", email);

    if (buscarUsuarioPorEmail(*lista, email) != NULL) {
        printf("Erro: ja existe um usuario com esse email.\n");
        return;
    }

    Usuario *novo = (Usuario *) malloc(sizeof(Usuario));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }
    strcpy(novo->email, email);
    lerString("Nome: ", novo->nome);

    novo->prox = *lista;   /* insere no início da lista (mais simples) */
    *lista = novo;
    printf("Usuario cadastrado com sucesso!\n");
}

void menuCadastro(Livro **livros, Usuario **usuarios, NoArvore **arvore, int *proximoId) {
    int op;
    do {
        printf("\n--- CADASTRO ---\n1. Livros\n2. Usuarios\n0. Voltar\nOpcao: ");
        op = lerInt();
        if (op == 1) cadastrarLivro(livros, arvore, proximoId);
        else if (op == 2) cadastrarUsuario(usuarios);
        else if (op != 0) printf("Opcao invalida.\n");
    } while (op != 0);
}












int main(void) {

  return 0;
}
