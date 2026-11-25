#include <stdio.h>
#include <stdlib.h>
#include "tabuleiro.h"

Tabuleiro* alocar(int linhas, int colunas, int nrBombas){
    Tabuleiro* t = malloc(sizeof(Tabuleiro));
    if (t == NULL){
        printf("Memoria insuficiente.\n");
        exit(1);
    }
    t->linhas = linhas;
    t->colunas = colunas;
    t->nrBombas = nrBombas;
    t->celulas = malloc(linhas * sizeof(Celula*));
    for (int i = 0; i < linhas; i++){
        t->celulas[i] = malloc(colunas * sizeof(Celula));
        for (int j = 0; j < colunas; j++){
            t->celulas[i][j].valor = 0;
            t->celulas[i][j].revelada = 0;
            t->celulas[i][j].marcada = 0;
        }
    }
    return t;
}

void liberar(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        free(t->celulas[i]);
    free(t->celulas);
    free(t);
}

int dentro(Tabuleiro* t, int i, int j){
    return i >= 0 && i < t->linhas && j >= 0 && j < t->colunas;
}

const char* nomeCasa(int linha, int coluna){
    /* Alguns nomes giram num rodizio de buffers para caber mais de um
       na mesma mensagem. */
    static char nomes[4][16];
    static int vez = 0;
    char* nome = nomes[vez];

    vez = (vez + 1) % 4;
    sprintf(nome, "%c%d", 'A' + coluna, linha + 1);
    return nome;
}

/* Quantas bombas existem nas oito casas ao redor. */
static int contarBombas(Tabuleiro* t, int i, int j){
    int total = 0;
    for (int a = i - 1; a <= i + 1; a++)
        for (int b = j - 1; b <= j + 1; b++)
            if (dentro(t, a, b) && t->celulas[a][b].valor == BOMBA)
                total++;
    return total;
}

void semear(Tabuleiro* t, int linhaSegura, int colunaSegura){
    /* Se sobrar espaco, a primeira jogada abre uma area inteira: deixamos o
       quadrado 3x3 em volta dela livre de bombas. */
    int reserva = (t->linhas * t->colunas - t->nrBombas > 9);
    int colocadas = 0;

    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            t->celulas[i][j].valor = 0;

    while (colocadas < t->nrBombas){
        int i = rand() % t->linhas;
        int j = rand() % t->colunas;
        int perto = (i >= linhaSegura - 1 && i <= linhaSegura + 1 &&
                     j >= colunaSegura - 1 && j <= colunaSegura + 1);

        if (t->celulas[i][j].valor == BOMBA)
            continue;
        if (reserva ? perto : (i == linhaSegura && j == colunaSegura))
            continue;

        t->celulas[i][j].valor = BOMBA;
        colocadas++;
    }

    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (t->celulas[i][j].valor != BOMBA)
                t->celulas[i][j].valor = contarBombas(t, i, j);
}

void limparJogadas(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++){
            t->celulas[i][j].revelada = 0;
            t->celulas[i][j].marcada = 0;
        }
}

int revelar(Tabuleiro* t, int i, int j){
    Celula* c = &t->celulas[i][j];

    if (c->revelada || c->marcada)
        return 1;
    c->revelada = 1;
    if (c->valor == BOMBA)
        return 0;

    /* Casa sem nenhuma bomba por perto abre a vizinhanca junto. */
    if (c->valor == 0)
        for (int a = i - 1; a <= i + 1; a++)
            for (int b = j - 1; b <= j + 1; b++)
                if (dentro(t, a, b))
                    revelar(t, a, b);
    return 1;
}

int marcar(Tabuleiro* t, int i, int j){
    if (t->celulas[i][j].revelada)
        return 0;
    t->celulas[i][j].marcada = !t->celulas[i][j].marcada;
    return 1;
}

int venceu(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (t->celulas[i][j].valor != BOMBA && !t->celulas[i][j].revelada)
                return 0;
    return 1;
}

int bombasRestantes(Tabuleiro* t){
    int marcadas = 0;
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (t->celulas[i][j].marcada)
                marcadas++;
    return t->nrBombas - marcadas;
}

void revelarBombas(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (t->celulas[i][j].valor == BOMBA)
                t->celulas[i][j].revelada = 1;
}
