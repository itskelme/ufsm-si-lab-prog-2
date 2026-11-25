#ifndef SOLVER_H
#define SOLVER_H

#include "tabuleiro.h"

typedef struct {
    int linha;          /* -1 quando nao sobrou jogada nenhuma */
    int coluna;
    char acao;          /* 'a' = abrir, 'm' = marcar */
    int chute;          /* 1 quando nao deu para provar, so estimar */
    const char* motivo;
} Jogada;

Jogada proximaJogada(Tabuleiro* t);

/* Sorteia as bombas ate cair um tabuleiro que o solver vence so na logica,
   sem precisar chutar uma vez sequer. */
void semearResolvivel(Tabuleiro* t, int linhaSegura, int colunaSegura);

#endif
