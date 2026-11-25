#ifndef TABULEIRO_H
#define TABULEIRO_H

#define BOMBA -1

typedef struct {
    int valor;      /* -1 = bomba, 0..8 = bombas vizinhas */
    int revelada;
    int marcada;
} Celula;

typedef struct {
    int linhas;
    int colunas;
    int nrBombas;
    Celula **celulas;
} Tabuleiro;

Tabuleiro* alocar(int linhas, int colunas, int nrBombas);
void liberar(Tabuleiro* t);

/* Sorteia as bombas depois da primeira jogada, longe da casa escolhida.
   Pode ser chamada varias vezes: cada chamada refaz o sorteio do zero. */
void semear(Tabuleiro* t, int linhaSegura, int colunaSegura);

/* Apaga o que ja foi aberto e marcado, mantendo as bombas onde estao. */
void limparJogadas(Tabuleiro* t);

int dentro(Tabuleiro* t, int i, int j);

/* Nome da casa como aparece na tela: coluna por letra, linha por numero. */
const char* nomeCasa(int linha, int coluna);

int revelar(Tabuleiro* t, int i, int j);   /* 0 = pisou numa bomba, 1 = seguiu vivo */

/* Poe ou tira a bandeira. Devolve 0 se a casa ja estava aberta. */
int marcar(Tabuleiro* t, int i, int j);

int venceu(Tabuleiro* t);
int bombasRestantes(Tabuleiro* t);
void revelarBombas(Tabuleiro* t);

#endif
