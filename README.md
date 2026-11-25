# Laboratório de Programação II

**UFSM · Sistemas de Informação · 2026/2** — Prof. Piveta · Linguagem C

## Trabalhos

### T1 — Campo minado (modo No-Guess)

[`trabalhos/t1-campo-minado`](trabalhos/t1-campo-minado) · versão entregue

Campo minado no terminal, a partir do modelo do professor (`Tabuleiro`, `alocar`, `liberar`, `preencher`, `imprimir`):

- tabuleiro com alocação dinâmica bidimensional e abertura em flood fill
- **modo No-Guess**: o campo só é aceito se um solver conseguir resolvê-lo inteiro por dedução, sem chute 50/50
- solver lógico (regra de 1 quadrado e redução de subconjuntos), dica para o jogador, modo manual e automático

```bash
cd trabalhos/t1-campo-minado
make            # ou: gcc -Wall -Wextra -std=c99 -O2 -o campo_minado tabuleiro.c solver.c main.c
./campo_minado
```

[`trabalhos/t1-campo-minado-versao-modular`](trabalhos/t1-campo-minado-versao-modular) é uma versão alternativa, dividida em `interface`, `jogo`, `solver` e `tabuleiro`:

```bash
cd trabalhos/t1-campo-minado-versao-modular
gcc -Wall -std=c99 -o campominado *.c
```

## Licença

[CC BY-NC 4.0](LICENSE): uso pessoal e modificação permitidos com atribuição; uso comercial proibido.
