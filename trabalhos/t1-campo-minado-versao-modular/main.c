#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabuleiro.h"
#include "interface.h"
#include "jogo.h"

int main(void){
    srand((unsigned int)time(NULL));

    int opcao;
    while ((opcao = lerMenu()) != 3){
        int linhas, colunas, nrBombas;
        lerConfiguracao(&linhas, &colunas, &nrBombas);

        Tabuleiro* t = alocar(linhas, colunas, nrBombas);
        if (opcao == 1)
            jogarHumano(t);
        else
            jogarComputador(t);
        liberar(t);
    }

    printf("Ate a proxima!\n");
    return 0;
}
