#ifndef INTERFACE_H
#define INTERFACE_H

#include "tabuleiro.h"

int  lerMenu(void);
void lerConfiguracao(int* linhas, int* colunas, int* nrBombas);
void lerJogada(Tabuleiro* t, int* i, int* j, char* acao);
int  perguntarSimNao(const char* pergunta);
void pausar(void);

void imprimirAjuda(void);
void avisarSorteio(void);
void imprimirTabuleiro(Tabuleiro* t, int segundos);
void imprimirFim(Tabuleiro* t, int ganhou, int segundos);

#endif
