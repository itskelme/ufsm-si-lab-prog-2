#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interface.h"

/* A tela usa so caracteres imprimiveis da tabela ASCII: nada de acento, de
   desenho de caixa em Unicode ou de cor. Assim o jogo sai igual em qualquer
   terminal, no Windows ou no Linux. */

#define LARGURA 38      /* largura interna das caixas do cabecalho */

static void lerLinha(char* destino, int tamanho){
    if (fgets(destino, tamanho, stdin) == NULL){
        printf("\n  Entrada encerrada.\n");
        exit(0);
    }
}

static int lerInteiro(const char* rotulo, int minimo, int maximo){
    char linha[32];
    int valor;

    while (1){
        printf("  %s (%d a %d): ", rotulo, minimo, maximo);
        lerLinha(linha, sizeof(linha));
        if (sscanf(linha, "%d", &valor) == 1 && valor >= minimo && valor <= maximo)
            return valor;
        printf("  Valor invalido, tente de novo.\n");
    }
}

static void barra(char traco){
    printf("  +");
    for (int k = 0; k < LARGURA; k++)
        putchar(traco);
    printf("+\n");
}

static void faixa(const char* texto){
    printf("  |%-*s|\n", LARGURA, texto);
}

/* O desenho de cada casa: escondida, com bandeira, vazia, numerada ou bomba. */
static char simbolo(Celula c){
    if (!c.revelada)
        return c.marcada ? 'F' : '.';
    if (c.valor == BOMBA)
        return '*';
    if (c.valor == 0)
        return ' ';
    return '0' + c.valor;
}

static void moldura(int colunas){
    printf("      +");
    for (int k = 0; k < colunas * 3 + 1; k++)
        putchar('-');
    printf("+\n");
}

void imprimirTabuleiro(Tabuleiro* t, int segundos){
    printf("\n  Bombas: %-3d         Tempo: %02d:%02d\n\n",
           bombasRestantes(t), segundos / 60, segundos % 60);

    printf("       ");
    for (int j = 0; j < t->colunas; j++)
        printf("  %c", 'A' + j);
    printf("\n");

    moldura(t->colunas);
    for (int i = 0; i < t->linhas; i++){
        printf("%5d |", i + 1);
        for (int j = 0; j < t->colunas; j++)
            printf("  %c", simbolo(t->celulas[i][j]));
        printf(" |\n");
    }
    moldura(t->colunas);
    printf("\n");
}

int lerMenu(void){
    printf("\n");
    barra('=');
    faixa("       C A M P O   M I N A D O");
    barra('=');
    faixa("  1 - Jogar");
    faixa("  2 - O computador resolve");
    faixa("  3 - Sair");
    barra('=');
    return lerInteiro("Opcao", 1, 3);
}

void lerConfiguracao(int* linhas, int* colunas, int* nrBombas){
    printf("\n");
    *linhas   = lerInteiro("Linhas", 2, 30);
    *colunas  = lerInteiro("Colunas", 2, 26);
    *nrBombas = lerInteiro("Bombas", 1, (*linhas) * (*colunas) - 1);
}

void imprimirAjuda(void){
    printf("\n");
    barra('-');
    faixa("  Como jogar:");
    faixa("    c3     abre a casa da coluna C,");
    faixa("           linha 3");
    faixa("    c3 m   poe ou tira a bandeira");
    barra('-');
}

void lerJogada(Tabuleiro* t, int* i, int* j, char* acao){
    char linha[32];

    while (1){
        char coluna, escolha = 'a';
        int numero;

        printf("  Jogada: ");
        lerLinha(linha, sizeof(linha));
        if (sscanf(linha, " %c%d %c", &coluna, &numero, &escolha) >= 2){
            *j = toupper(coluna) - 'A';
            *i = numero - 1;
            *acao = tolower(escolha);
            if (dentro(t, *i, *j) && (*acao == 'a' || *acao == 'm'))
                return;
        }
        printf("  Nao entendi. Use c3 para abrir ou c3 m para marcar.\n");
    }
}

int perguntarSimNao(const char* pergunta){
    char linha[32];

    printf("  %s (s/n): ", pergunta);
    lerLinha(linha, sizeof(linha));
    return linha[0] == 's' || linha[0] == 'S';
}

void pausar(void){
    char linha[32];

    printf("  Enter para a proxima jogada...");
    lerLinha(linha, sizeof(linha));
}

void imprimirFim(Tabuleiro* t, int ganhou, int segundos){
    char tempo[32];

    revelarBombas(t);
    imprimirTabuleiro(t, segundos);
    sprintf(tempo, "  Tempo total: %02d:%02d", segundos / 60, segundos % 60);

    barra('=');
    faixa(ganhou ? "  VOCE LIMPOU O CAMPO MINADO!" : "  BOOM! Voce pisou numa bomba.");
    faixa(tempo);
    barra('=');
}

void avisarSorteio(void){
    printf("  Montando um campo que da para resolver so na logica...\n");
    fflush(stdout);
}
