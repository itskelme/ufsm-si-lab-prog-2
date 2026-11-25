#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabuleiro.h"
#include "solver.h"

#ifdef _WIN32
  #include <windows.h>
  #define DORMIR(ms) Sleep(ms)
#else
  #include <unistd.h>
  #define DORMIR(ms) usleep((ms) * 1000)
#endif

#define LEU_OK   1
#define LEU_ERRO 0
#define LEU_FIM (-1)


static void LimpaBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}


/* Devolve LEU_OK, LEU_ERRO (entrada invalida) ou LEU_FIM (EOF).
   Sem tratar o EOF os menus entravam em laco infinito quando a entrada acabava. */
static int LerInteiro(int *destino) {
    int r = scanf("%d", destino);
    if (r == EOF) return LEU_FIM;
    if (r != 1) {
        LimpaBuffer();
        return LEU_ERRO;
    }
    return LEU_OK;
}


static int LerCaractere(char *destino) {
    int r = scanf(" %c", destino);
    if (r == EOF) return LEU_FIM;
    if (r != 1) {
        LimpaBuffer();
        return LEU_ERRO;
    }
    return LEU_OK;
}


static void AvisaGeracao(Tabuleiro *t) {
    if (!t->gerado || t->semChute) return;
    printf("\nNao foi possivel gerar um tabuleiro 100%% resolvivel e logico\n");
    printf("        com essas dimensoes e essa quantidade de bombas.\n");
    printf("        Em algum momento pode ser necessario chutar.\n");
}


static void ModoJogador(Tabuleiro *t) {
    int fim = 0;
    int venceu = 0;
    int avisou = 0;

    printf("\nComandos:\n");
    printf("  r L C -> Revelar celula (ex: r 2 3)\n");
    printf("  b L C -> Colocar/remover bandeira (ex: b 2 3)\n");
    printf("  s     -> Dica da proxima jogada\n");
    printf("  q     -> Sair para o menu\n");

    while (!fim) {
        printf("\nBombas restantes: %d", t->nrBombas - ContaBandeiras(t));
        Imprimir(t, 0);

        printf("Comando: ");
        char comando;
        int r = LerCaractere(&comando);
        if (r == LEU_FIM) {
            printf("\nEntrada encerrada.\n");
            return;
        }
        if (r == LEU_ERRO) continue;

        if (comando == 'q' || comando == 'Q') {
            printf("Jogo cancelado.\n");
            return;
        }

        if (comando == 's' || comando == 'S') {
            int l, col, acao;
            if (Dica(t, &l, &col, &acao)) {
                printf("\n[DICA] Posicao (%d, %d): %s.\n",
                       l, col, (acao == 1 ? "ABRIR" : "MARCAR BANDEIRA"));
            } else {
                printf("\n[DICA] Sem jogadas logicas imediatas.\n");
            }
            continue;
        }

        /* valida o comando antes de consumir as coordenadas, senao uma letra
           desconhecida engolia a linha seguinte */
        if (comando != 'r' && comando != 'R' && comando != 'b' && comando != 'B') {
            LimpaBuffer();
            printf("Comando desconhecido. Use r, b, s ou q.\n");
            continue;
        }

        int l, col;
        if (LerInteiro(&l) != LEU_OK || LerInteiro(&col) != LEU_OK) {
            printf("Coordenadas invalidas.\n");
            continue;
        }

        if (!PosicaoValida(t, l, col)) {
            printf("Posicao fora do tabuleiro (%dx%d)!\n", t->linhas, t->colunas);
            continue;
        }

        if (comando == 'b' || comando == 'B') {
            MudaBandeira(t, l, col);
        } else {
            if (t->estado[l][col] == 2) {
                printf("Tire a bandeira antes de abrir neh, fica esperto maninho\n");
                continue;
            }

            if (!Abrir(t, l, col)) {
                fim = 1;
                venceu = 0;
            } else if (Ganhou(t)) {
                fim = 1;
                venceu = 1;
            }

            if (!avisou) {
                avisou = 1;
                AvisaGeracao(t);
            }
        }
    }

    if (venceu) {
        printf("\nParabens! Voce venceuuuu o Campo Minado!!! Tash esperando o oscar?\n");
    } else {
        printf("\nBomba! Fim de jogo.\n");
    }
    Imprimir(t, 1);
}


/* O primeiro clique so fica com o 3x3 blindado se sobrar espaco para as bombas
   fora dele. Em tabuleiros pequenos o centro nao serve; um canto costuma servir. */
static void EscolhePrimeiroClique(Tabuleiro *t, int *l_out, int *c_out) {
    int cand_l[5], cand_c[5], n = 0;

    cand_l[n] = t->linhas / 2;   cand_c[n] = t->colunas / 2;   n++;
    cand_l[n] = 0;               cand_c[n] = 0;                n++;
    cand_l[n] = 0;               cand_c[n] = t->colunas - 1;   n++;
    cand_l[n] = t->linhas - 1;   cand_c[n] = 0;                n++;
    cand_l[n] = t->linhas - 1;   cand_c[n] = t->colunas - 1;   n++;

    int total = t->linhas * t->colunas;
    for (int i = 0; i < n; i++) {
        if (total - TamanhoZonaSegura(t, cand_l[i], cand_c[i]) >= t->nrBombas) {
            *l_out = cand_l[i];
            *c_out = cand_c[i];
            return;
        }
    }

    *l_out = cand_l[0];
    *c_out = cand_c[0];
}


static void ModoComputador(Tabuleiro *t) {
    int passos = 0;

    printf("\nO computador esta rodando o  algoritmo que hackeia o nosso tabuleiro e resolve sozinho bixo... e\n");

    if (t->abertas == 0) {
        int ini_l, ini_c;
        EscolhePrimeiroClique(t, &ini_l, &ini_c);
        passos++;
        Abrir(t, ini_l, ini_c);
        AvisaGeracao(t);
        printf("\n[Passo %d] Abre em (%d, %d)", passos, ini_l, ini_c);
        Imprimir(t, 0);
        DORMIR(120);
    }

    while (!Ganhou(t)) {
        int l = 0, col = 0, acao = 0;
        if (!ProximoPasso(t, &l, &col, &acao)) {
            printf("\nSem jogadas logicas restantes, daqui em diante so seria chute em cachorro morto amor\n");
            Imprimir(t, 1);
            return;
        }

        passos++;
        if (acao == 2) {
            MudaBandeira(t, l, col);
            printf("\n[Passo %d] Bandeira em (%d, %d)", passos, l, col);
        } else {
            if (!Abrir(t, l, col)) {
                printf("\n[Passo %d] O computador atingiu uma bomba em (%d, %d).\n", passos, l, col);
                Imprimir(t, 1);
                return;
            }
            printf("\n[Passo %d] Abre em (%d, %d)", passos, l, col);
        }

        Imprimir(t, 0);
        DORMIR(120);
    }

    printf("\nCampo Minado resolvido com sucesso em %d passos!!\n", passos);
    Imprimir(t, 1);
}


int main(void) {
#ifdef _WIN32
    /* nosso tabuleiro 3000 super power usa caracteres de moldura UTF-8, e dai sem isso o console legado
       (cp850/cp1252) mostra lixo no lugar das bordas infelizmente mona */
    SetConsoleOutputCP(CP_UTF8);
#endif

    srand((unsigned int)time(NULL));

    while (1) {
        printf("\n--- CAMPO MINADO 3000 SUPERPOWER DE LAB2 ---\n");
        
        printf("Escolhe seu triste destino:\n");
        
        printf("1. Vc vai jogar manualmente\n");
        printf("2. A gente usa hack e o computador resolve na marra e sem IA \n");
        printf("3. Vc desiste da vida e resolve sair do jogo\n");
        printf("Opcao: ");

        int opcao;
        int r = LerInteiro(&opcao);
        if (r == LEU_FIM) {
            printf("\nLeitura da entrada concluida\n");
            break;
        }
        if (r == LEU_ERRO) {
            printf("Opcao invalida!\n");
            continue;
        }

        if (opcao == 3) {
            printf("Ate a proxima!\n");
            break;
        }

        if (opcao != 1 && opcao != 2) {
            printf("Opcao invalida.\n");
            continue;
        }

        int nrLinhas, nrColunas, nrBombas;
        printf("\nNumero de linhas: ");
        r = LerInteiro(&nrLinhas);
        if (r == LEU_FIM) break;
        if (r != LEU_OK || nrLinhas <= 0) {
            printf("Numero de linhas invalido.\n");
            continue;
        }

        printf("Numero de colunas: ");
        r = LerInteiro(&nrColunas);
        if (r == LEU_FIM) break;
        if (r != LEU_OK || nrColunas <= 0) {
            printf("Numero de colunas eh invalido bb\n");
            continue;
        }

        if (nrLinhas * nrColunas < 2) {
            printf("O tabuleiro precisa de pelo menos 2 celulas gato\n");
            continue;
        }

        printf("Numero de bombas (1 a %d): ", (nrLinhas * nrColunas) - 1);
        r = LerInteiro(&nrBombas);
        if (r == LEU_FIM) break;
        if (r != LEU_OK || nrBombas <= 0 || nrBombas >=  nrLinhas * nrColunas) {
            printf("Quantidade de bombas invalidada kskslks, tenta outra numero pprt\n");
            continue;
        }

        Tabuleiro* t = Alocar(nrLinhas, nrColunas, nrBombas);
        if (!t) {
            printf("Erro ao alocar memoria para o tabuleiro, pc da xuxa nao roda\n");
            continue;
        }

        if (opcao == 1) {
            ModoJogador(t);
        } else {
            ModoComputador(t);
        }

        Liberar(t);
    }

    return 0;
}
