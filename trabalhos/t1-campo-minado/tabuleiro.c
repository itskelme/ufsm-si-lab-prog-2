#include "tabuleiro.h"
#include "solver.h"
#include <time.h>


int PosicaoValida(Tabuleiro* t, int l, int c) {
    if(!t) return 0;
    return (l >= 0 && l < t->linhas && c >= 0 && c < t->colunas);
}


Tabuleiro* Alocar(int linhas, int colunas, int bombas) {
    if(linhas <= 0 || colunas <= 0 || bombas <= 0 || bombas >= linhas * colunas) {
        return NULL;
    }

    Tabuleiro* t = (Tabuleiro*)malloc(sizeof(Tabuleiro));
    if(!t) return NULL;

    t->linhas = linhas;
    t->colunas = colunas;
    t->nrBombas = bombas;
    t->abertas = 0;
    t->gerado = 0;
    t->semChute = 0;

    t->celulas = (int**)calloc((size_t)linhas, sizeof(int*));
    t->estado = (int**)calloc((size_t)linhas, sizeof(int*));
    if (!t->celulas || !t->estado) {
        Liberar(t);
        return NULL;
    }

    for (int i = 0; i < t->linhas; i++) {
        t->celulas[i] = (int*)calloc((size_t)colunas, sizeof(int));
        t->estado[i] = (int*)calloc((size_t)colunas, sizeof(int));
        if (!t->celulas[i] || !t->estado[i]) {
            Liberar(t);
            return NULL;
        }
    }

    return t;
}


void Liberar(Tabuleiro* t) {
    if (!t) return;

    if (t->celulas) {
        for(int i = 0; i < t->linhas; i++) {
            free(t->celulas[i]);
        }
        free(t->celulas);
    }

    if (t->estado) {
        for (int i = 0; i < t->linhas; i++) {
            free(t->estado[i]);
        }
        free(t->estado);
    }

    free(t);
}


/* Quantas celulas o bloco 3x3 em volta do primeiro clique realmente cobre
   (o bloco e' recortado nas bordas, entao nem sempre sao 9). */
int TamanhoZonaSegura(Tabuleiro* t, int safe_l, int safe_c) {
    int n = 0;
    for (int dl = -1; dl <= 1; dl++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (PosicaoValida(t, safe_l + dl, safe_c + dc)) n++;
        }
    }
    return n;
}


static void SorteiaBombas(Tabuleiro* t, int safe_l, int safe_c) {
    for (int i = 0; i < t->linhas; i++) {
        for (int j = 0; j < t->colunas; j++) {
            t->celulas[i][j] = 0;
        }
    }

    /* So da' para blindar o 3x3 inteiro se sobrar espaco para todas as bombas.
       Senao, protege apenas a celula clicada. */
    int total = t->linhas * t->colunas;
    int zona = TamanhoZonaSegura(t, safe_l, safe_c);
    int usaZona = (total - zona >= t->nrBombas);

    int colocadas = 0;
    while(colocadas < t->nrBombas) {
        int l = rand() % t->linhas;
        int c = rand() % t->colunas;

        if (usaZona) {
            if (abs(l - safe_l) <= 1 && abs(c - safe_c) <= 1) continue;
        } else {
            if (l == safe_l && c == safe_c) continue;
        }

        if (t->celulas[l][c] != -1) {
            t->celulas[l][c] = -1;
            colocadas++;
        }
    }

    // preenche os vizinhos
    for (int i = 0; i < t->linhas; i++) {
        for (int j = 0; j < t->colunas; j++) {
            if (t->celulas[i][j] == -1) continue;

            int count = 0;
            for(int di = -1; di <= 1; di++) {
                for(int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (PosicaoValida(t, ni, nj) && t->celulas[ni][nj] == -1) {
                        count++;
                    }
                }
            }
            t->celulas[i][j] = count;
        }
    }
}


void Preencher(Tabuleiro* t, int safe_l, int safe_c) {
    const int limite = 2000;
    const clock_t orcamento = 3 * CLOCKS_PER_SEC;

    if (!t) return;

    clock_t inicio = clock();
    int tentativas = 0;

    t->semChute = 0;
    do {
        SorteiaBombas(t, safe_l, safe_c);
        tentativas++;
        if (TestaSolucao(t, safe_l, safe_c)) {
            t->semChute = 1;
            break;
        }
    } while(tentativas < limite && (clock() - inicio) < orcamento);

    t->gerado = 1;
}


int Abrir(Tabuleiro* t, int l, int c) {
    if (!PosicaoValida(t, l, c)) return 1;
    if (t->estado[l][c] == 1 || t->estado[l][c] == 2) return 1;

    if (!t->gerado) {
        Preencher(t, l, c);
    }

    if (t->celulas[l][c] == -1) {
        t->estado[l][c] = 1;
        return 0; // pisou na bomba
    }

    /* Flood fill iterativo: a versao recursiva estourava a pilha em
       tabuleiros grandes (a partir de ~150x150 com poucas bombas). */
    int total = t->linhas * t->colunas;
    int *pilha = (int*)malloc((size_t)total * sizeof(int));

    t->estado[l][c] = 1;
    t->abertas++;

    if (!pilha) return 1; /* sem memoria: abre so a celula pedida */

    int topo = 0;
    pilha[topo++] = l * t->colunas + c;

    while (topo > 0) {
        int p = pilha[--topo];
        int pl = p / t->colunas;
        int pc = p % t->colunas;

        if (t->celulas[pl][pc] != 0) continue;

        for (int dl = -1; dl <= 1; dl++) {
            for (int dc = -1; dc <= 1; dc++) {
                int nl = pl + dl;
                int nc = pc + dc;
                if (PosicaoValida(t, nl, nc) && t->estado[nl][nc] == 0) {
                    t->estado[nl][nc] = 1;
                    t->abertas++;
                    pilha[topo++] = nl * t->colunas + nc;
                }
            }
        }
    }

    free(pilha);
    return 1;
}


void MudaBandeira(Tabuleiro* t, int l, int c) {
    if (!PosicaoValida(t, l, c) || t->estado[l][c] == 1) return;
    t->estado[l][c] = (t->estado[l][c] == 2 ? 0 : 2);
}


int Ganhou(Tabuleiro* t) {
    if (!t) return 0;
    int total = t->linhas * t->colunas;
    return (t->abertas == (total - t->nrBombas));
}


int ContaBandeiras(Tabuleiro* t) {
    if (!t) return 0;
    int n = 0;
    for (int i = 0; i < t->linhas; i++) {
        for (int j = 0; j < t->colunas; j++) {
            if (t->estado[i][j] == 2) n++;
        }
    }
    return n;
}


void ContaVizinhos(Tabuleiro* t, int l, int c, int *ocultos, int *bandeiras) {
    *ocultos = 0;
    *bandeiras = 0;

    for(int dl = -1; dl <= 1; dl++) {
        for(int dc = -1; dc <= 1; dc++) {
            if (dl == 0 && dc == 0) continue;
            int nl = l + dl;
            int nc = c + dc;
            if (PosicaoValida(t, nl, nc)) {
                if (t->estado[nl][nc] == 2) (*bandeiras)++;
                else if (t->estado[nl][nc] == 0) (*ocultos)++;
            }
        }
    }
}


void Imprimir(Tabuleiro* t, int revelado) {
    if (!t) return;

    printf("\n    ");
    for (int j = 0; j < t->colunas; j++) printf(" %2d ", j);
    printf("\n    ┌");
    for (int j = 0; j < t->colunas; j++) printf("───%s", (j < t->colunas - 1 ? "┬" : "┐\n"));

    for (int i = 0; i < t->linhas; i++) {
        printf(" %2d │", i);
        for (int j = 0; j < t->colunas; j++) {
            if (revelado) {
                if (t->celulas[i][j] == -1) printf(" * │");
                else if (t->celulas[i][j] > 0) printf(" %d │", t->celulas[i][j]);
                else printf("   │");
            } else {
                if (t->estado[i][j] == 2) {
                    printf(" P │");
                } else if (t->estado[i][j] == 0) {
                    printf(" . │");
                } else if (t->celulas[i][j] == -1) {
                    printf(" * │");
                } else if (t->celulas[i][j] == 0) {
                    printf("   │");
                } else {
                    printf(" %d │", t->celulas[i][j]);
                }
            }
        }
        printf("\n");
        if (i < t->linhas - 1) {
            printf("    ├");
            for(int j = 0; j < t->colunas; j++) printf("───%s", (j < t->colunas - 1 ? "┼" : "┤\n"));
        }
    }
    printf("    └");
    for (int j = 0; j < t->colunas; j++) printf("───%s", (j < t->colunas - 1 ? "┴" : "┘\n\n"));
}
