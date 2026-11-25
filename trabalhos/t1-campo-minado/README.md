# Campo Minado (C99 - Modo No-Guess)

Implementacao em C baseada no modelo do professor (`Tabuleiro`, `alocar`, `liberar`, `preencher`, `imprimir`) com resolucao 100% deterministica sem situacoes de puro azar (50/50), conforme o artigo da Wikipedia.

## Estrutura do Projeto

- `tabuleiro.h` e `tabuleiro.c`: Estrutura `Tabuleiro`, alocacao dinamica bidimensional (`int **celulas`, `int **estado`), abertura em flood fill iterativo, contagem de vizinhanca e gerador No-Guess.
- `solver.h` e `solver.c`: Motor de deducao logica (analise de 1 quadrado e reducao de subconjuntos de 2 quadrados), dica segura para o jogador (`Dica`) e verificador de solvabilidade estrita (`TestaSolucao`).
- `main.c`: Menu principal, leitura das dimensoes e numero de bombas, modo manual e modo automatico.

## Compilacao e Execucao

```bash
make
./campo_minado        # no Windows: campo_minado.exe
```

Sem `make`, compile direto:

```bash
gcc -Wall -Wextra -std=c99 -O2 -o campo_minado tabuleiro.c solver.c main.c
```

## Sobre o modo No-Guess

O gerador sorteia o campo depois do primeiro clique e so aceita um tabuleiro que o
solver consiga resolver do inicio ao fim apenas por deducao. Ele tenta ate 2000
sorteios (com orcamento de 3 segundos).

Nem toda combinacao de tamanho e numero de bombas admite um tabuleiro sem chute -
um 2x2 com 1 bomba, por exemplo, e impossivel. Nesses casos o programa exibe um
aviso explicito em vez de prometer um campo deterministico que nao existe.
