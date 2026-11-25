#include <stdio.h>
#include <time.h>
#include "jogo.h"
#include "interface.h"
#include "solver.h"

static int decorridos(time_t inicio){
    return (int)difftime(time(NULL), inicio);
}

/* As bombas so entram no tabuleiro depois da primeira jogada. Alem de ninguem
   perder de cara, isso deixa escolher um sorteio que se resolve na logica. */
static int abrir(Tabuleiro* t, int i, int j, int* semeado){
    if (!*semeado){
        avisarSorteio();
        semearResolvivel(t, i, j);
        *semeado = 1;
    }
    return revelar(t, i, j);
}

void jogarHumano(Tabuleiro* t){
    time_t inicio = time(NULL);
    int semeado = 0;
    int vivo = 1;

    imprimirAjuda();
    while (vivo && !venceu(t)){
        int i, j;
        char acao;

        imprimirTabuleiro(t, decorridos(inicio));
        lerJogada(t, &i, &j, &acao);

        if (acao == 'm'){
            if (!marcar(t, i, j))
                printf("  A casa %s ja esta aberta.\n", nomeCasa(i, j));
        }
        else if (t->celulas[i][j].marcada)
            printf("  Tire a bandeira de %s antes de abrir.\n", nomeCasa(i, j));
        else
            vivo = abrir(t, i, j, &semeado);
    }
    imprimirFim(t, vivo, decorridos(inicio));
}

void jogarComputador(Tabuleiro* t){
    time_t inicio = time(NULL);
    int semeado = 0;
    int vivo = 1;
    int passoAPasso = perguntarSimNao("Parar a cada jogada");

    while (vivo && !venceu(t)){
        Jogada jogada = proximaJogada(t);
        if (jogada.linha < 0)
            break;

        imprimirTabuleiro(t, decorridos(inicio));
        printf("  Computador %s %s: %s\n",
               jogada.acao == 'm' ? "marca" : "abre",
               nomeCasa(jogada.linha, jogada.coluna), jogada.motivo);
        if (passoAPasso)
            pausar();

        if (jogada.acao == 'm')
            marcar(t, jogada.linha, jogada.coluna);
        else
            vivo = abrir(t, jogada.linha, jogada.coluna, &semeado);
    }
    imprimirFim(t, vivo && venceu(t), decorridos(inicio));
}
