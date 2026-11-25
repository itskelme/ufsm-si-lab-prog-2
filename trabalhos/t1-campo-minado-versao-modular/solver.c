#include <stdio.h>
#include "solver.h"

/* O computador joga por deducao. Primeiro esgota o que da para provar olhando
   os numeros da tela; so quando nao sobra nenhuma certeza ele estima, e mesmo
   assim escolhendo a casa de menor risco. */

#define TENTATIVAS 25000    /* sorteios ate desistir de um tabuleiro perfeito */

static char explicacao[80];

/* As casas escondidas em volta de um numero: no maximo oito. */
typedef struct {
    int linha[8];
    int coluna[8];
    int total;
} Vizinhanca;

static Vizinhanca ocultasDe(Tabuleiro* t, int i, int j){
    Vizinhanca v;

    v.total = 0;
    for (int a = i - 1; a <= i + 1; a++)
        for (int b = j - 1; b <= j + 1; b++)
            if (dentro(t, a, b) && !t->celulas[a][b].revelada && !t->celulas[a][b].marcada){
                v.linha[v.total] = a;
                v.coluna[v.total] = b;
                v.total++;
            }
    return v;
}

static int bandeirasDe(Tabuleiro* t, int i, int j){
    int total = 0;
    for (int a = i - 1; a <= i + 1; a++)
        for (int b = j - 1; b <= j + 1; b++)
            if (dentro(t, a, b) && t->celulas[a][b].marcada)
                total++;
    return total;
}

static int contem(Vizinhanca* v, int linha, int coluna){
    for (int k = 0; k < v->total; k++)
        if (v->linha[k] == linha && v->coluna[k] == coluna)
            return 1;
    return 0;
}

static int contida(Vizinhanca* menor, Vizinhanca* maior){
    for (int k = 0; k < menor->total; k++)
        if (!contem(maior, menor->linha[k], menor->coluna[k]))
            return 0;
    return 1;
}

static void primeiraFora(Vizinhanca* maior, Vizinhanca* menor, int* li, int* lj){
    for (int k = 0; k < maior->total; k++)
        if (!contem(menor, maior->linha[k], maior->coluna[k])){
            *li = maior->linha[k];
            *lj = maior->coluna[k];
            return;
        }
}

static int numero(Tabuleiro* t, int i, int j){
    return t->celulas[i][j].revelada && t->celulas[i][j].valor > 0;
}

static int tabuleiroIntocado(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (t->celulas[i][j].revelada)
                return 0;
    return 1;
}

static Jogada montar(int linha, int coluna, char acao, int chute, const char* motivo){
    Jogada jogada;

    jogada.linha = linha;
    jogada.coluna = coluna;
    jogada.acao = acao;
    jogada.chute = chute;
    jogada.motivo = motivo;
    return jogada;
}

static Jogada nada(void){
    return montar(-1, -1, 'a', 0, "sem jogadas possiveis");
}

/* Um numero de cada vez: ou tudo em volta e bomba, ou ninguem mais e. */
static Jogada olharNumeros(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++){
            if (!numero(t, i, j))
                continue;

            Vizinhanca ocultas = ocultasDe(t, i, j);
            if (ocultas.total == 0)
                continue;

            int faltam = t->celulas[i][j].valor - bandeirasDe(t, i, j);
            if (faltam == ocultas.total){
                sprintf(explicacao, "o %d de %s so tem bomba em volta",
                        t->celulas[i][j].valor, nomeCasa(i, j));
                return montar(ocultas.linha[0], ocultas.coluna[0], 'm', 0, explicacao);
            }
            if (faltam == 0){
                sprintf(explicacao, "o %d de %s ja esta satisfeito",
                        t->celulas[i][j].valor, nomeCasa(i, j));
                return montar(ocultas.linha[0], ocultas.coluna[0], 'a', 0, explicacao);
            }
        }
    return nada();
}

/* Dois numeros vizinhos: quando um enxerga so um pedaco do que o outro
   enxerga, a diferenca entre eles entrega o resto. E o padrao 1-2. */
static Jogada compararNumeros(Tabuleiro* t){
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++){
            if (!numero(t, i, j))
                continue;

            Vizinhanca menor = ocultasDe(t, i, j);
            if (menor.total == 0)
                continue;
            int faltamMenor = t->celulas[i][j].valor - bandeirasDe(t, i, j);

            for (int a = i - 2; a <= i + 2; a++)
                for (int b = j - 2; b <= j + 2; b++){
                    if (!dentro(t, a, b) || (a == i && b == j) || !numero(t, a, b))
                        continue;

                    Vizinhanca maior = ocultasDe(t, a, b);
                    if (maior.total <= menor.total || !contida(&menor, &maior))
                        continue;

                    int faltamMaior = t->celulas[a][b].valor - bandeirasDe(t, a, b);
                    int sobra = maior.total - menor.total;
                    int li = -1, lj = -1;

                    /* Toda a diferenca entre os dois e bomba. */
                    if (faltamMaior - faltamMenor == sobra){
                        primeiraFora(&maior, &menor, &li, &lj);
                        sprintf(explicacao, "%s e %s juntos so deixam bomba de fora",
                                nomeCasa(i, j), nomeCasa(a, b));
                        return montar(li, lj, 'm', 0, explicacao);
                    }
                    /* O numero de fora nao precisa de nenhuma bomba a mais. */
                    if (faltamMaior == faltamMenor){
                        primeiraFora(&maior, &menor, &li, &lj);
                        sprintf(explicacao, "%s ja explica o %s: o resto e seguro",
                                nomeCasa(i, j), nomeCasa(a, b));
                        return montar(li, lj, 'a', 0, explicacao);
                    }
                }
        }
    return nada();
}

/* Chance de (i,j) ser bomba: vale o pior palpite entre os numeros vizinhos, e
   a densidade do que sobrou quando a casa nao encosta em numero nenhum. */
static double risco(Tabuleiro* t, int i, int j, double densidade){
    double pior = -1.0;

    for (int a = i - 1; a <= i + 1; a++)
        for (int b = j - 1; b <= j + 1; b++)
            if (dentro(t, a, b) && numero(t, a, b)){
                Vizinhanca ocultas = ocultasDe(t, a, b);
                if (ocultas.total > 0){
                    double chance = (double)(t->celulas[a][b].valor - bandeirasDe(t, a, b))
                                    / ocultas.total;
                    if (chance > pior)
                        pior = chance;
                }
            }
    return pior < 0 ? densidade : pior;
}

static Jogada chutar(Tabuleiro* t){
    int escondidas = 0, li = -1, lj = -1;
    double densidade, menor = 2.0;

    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            if (!t->celulas[i][j].revelada && !t->celulas[i][j].marcada)
                escondidas++;
    if (escondidas == 0)
        return nada();

    densidade = (double)bombasRestantes(t) / escondidas;
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++){
            if (t->celulas[i][j].revelada || t->celulas[i][j].marcada)
                continue;
            double chance = risco(t, i, j, densidade);
            if (chance < menor){
                menor = chance;
                li = i;
                lj = j;
            }
        }

    sprintf(explicacao, "chute de menor risco (%.0f%% de bomba)", menor * 100);
    return montar(li, lj, 'a', 1, explicacao);
}

Jogada proximaJogada(Tabuleiro* t){
    /* Tela em branco: o canto e a casa com menos vizinhos, e menos risco. */
    if (tabuleiroIntocado(t))
        return montar(0, 0, 'a', 0, "abertura pelo canto");

    Jogada jogada = olharNumeros(t);
    if (jogada.linha >= 0)
        return jogada;

    jogada = compararNumeros(t);
    if (jogada.linha >= 0)
        return jogada;

    return chutar(t);
}

/* Joga a partida inteira em silencio, so para saber se este sorteio se
   resolve sem chute. No fim apaga as jogadas e deixa as bombas onde estao. */
static int resolveNaLogica(Tabuleiro* t, int linhaSegura, int colunaSegura){
    int limpo = 1;

    revelar(t, linhaSegura, colunaSegura);
    while (!venceu(t)){
        Jogada jogada = proximaJogada(t);
        if (jogada.linha < 0 || jogada.chute){
            limpo = 0;
            break;
        }
        if (jogada.acao == 'm')
            marcar(t, jogada.linha, jogada.coluna);
        else
            revelar(t, jogada.linha, jogada.coluna);
    }
    limparJogadas(t);
    return limpo;
}

void semearResolvivel(Tabuleiro* t, int linhaSegura, int colunaSegura){
    for (int tentativa = 0; tentativa < TENTATIVAS; tentativa++){
        semear(t, linhaSegura, colunaSegura);
        if (resolveNaLogica(t, linhaSegura, colunaSegura))
            return;
    }
    /* Nao apareceu um tabuleiro perfeito a tempo: vale o ultimo sorteado. */
}
