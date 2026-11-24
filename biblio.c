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













int main(void) {

  return 0;
}
