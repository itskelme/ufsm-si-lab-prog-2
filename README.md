# Laboratório de Programação II

> Trabalhos de Laboratório de Programação II em C. O T1 é um campo minado de terminal que só gera tabuleiros que se resolvem por dedução (modo No-Guess) e que pode ser resolvido pelo computador. O T3 é um sistema de vendas com clientes, produtos, vendas e relatórios, guardados em listas encadeadas e em arquivo texto.

**Contexto:** UFSM · Sistemas de Informação · Laboratório de Programação II · Prof. Piveta · 2026/2

## Sobre

O T1 parte do modelo de campo minado dado pelo professor (a estrutura `Tabuleiro` e as funções de alocar, liberar, preencher e imprimir) e o completa com jogo, solver e gerador. A ideia central é o **No-Guess**: as bombas só são sorteadas depois da primeira jogada, e o sorteio só é aceito se um solver lógico conseguir resolver o tabuleiro inteiro sem chutar. Assim o jogador nunca cai em um 50/50 de pura sorte.

O trabalho pratica alocação dinâmica de matriz (`int **`), structs, modularização em `.c`/`.h`, busca em vizinhança, flood fill com pilha explícita e um pequeno motor de dedução.

Há duas versões:

- [`trabalhos/t1-campo-minado`](trabalhos/t1-campo-minado): **versão entregue**, com Makefile.
- [`trabalhos/t1-campo-minado-versao-modular`](trabalhos/t1-campo-minado-versao-modular): reescrita alternativa, dividida em interface, jogo, solver e tabuleiro, com outra forma de digitar as jogadas e um solver que explica cada passo.

O T3 ([`trabalhos/t3-sistema-de-vendas`](trabalhos/t3-sistema-de-vendas)) é um sistema de vendas: cadastro, consulta, alteração e ordenação de clientes e produtos, registro de vendas ligadas a um cliente e aos produtos comprados, e oito relatórios. Cada coleção é uma lista encadeada, cada venda tem a sua própria lista de itens, e tudo é gravado em arquivo texto a cada alteração. Pratica listas encadeadas (inserção no fim, inserção ordenada, remoção, ordenação mexendo só nos ponteiros), arquivos texto e modularização.

## Funcionalidades

### Versão entregue (`t1-campo-minado`)

- Menu com três opções: jogar, deixar o computador resolver ou sair.
- Tamanho livre: qualquer número de linhas e colunas (com pelo menos 2 células) e de 1 a `linhas*colunas - 1` bombas.
- Tabuleiro alocado dinamicamente: `int **celulas` guarda `-1` para bomba e `0..8` para o número de vizinhas, e `int **estado` guarda `0` fechada, `1` aberta e `2` bandeira.
- As bombas são sorteadas no primeiro clique. O quadrado 3x3 em volta do clique fica livre de bombas quando há espaço; se não há, só a célula clicada é protegida.
- **Gerador No-Guess:** sorteia até 2000 tabuleiros (ou até 3 segundos) e fica com o primeiro que o solver resolve do início ao fim. Se nenhum servir, por exemplo em um 2x2 com 1 bomba, o jogo avisa que pode ser preciso chutar.
- **Solver lógico** em duas regras:
  1. *Um número:* se as bombas que faltam são iguais ao número de casas fechadas em volta, todas são bombas. Se não falta nenhuma, todas as casas fechadas são seguras.
  2. *Dois números (subconjunto):* se as casas fechadas de um número estão todas entre as de um vizinho a até 2 casas, a diferença entre os dois números diz se as casas que sobram são todas seguras ou todas bombas.
- **Dica (`s`):** refaz a dedução do zero a partir das casas abertas, sem confiar nas bandeiras do jogador (que podem estar erradas), e indica a próxima jogada segura.
- **Modo automático:** o computador escolhe o primeiro clique (o centro, ou um canto onde caiba a zona segura), aplica o solver passo a passo com uma pausa de 120 ms e imprime o tabuleiro a cada jogada. Para quando não há mais jogada lógica.
- Abertura de áreas vazias por flood fill iterativo com pilha alocada (a versão recursiva estourava a pilha a partir de uns 150x150).
- Leitura de entrada que trata dado inválido e fim de arquivo (EOF), então o programa não entra em laço infinito.
- O tabuleiro é desenhado com moldura Unicode. No Windows o console é trocado para UTF-8 (`SetConsoleOutputCP`).

### Versão modular (`t1-campo-minado-versao-modular`)

- Cada célula é uma struct `Celula { valor, revelada, marcada }` dentro de uma matriz `Celula **`.
- Tabuleiro de 2 a 30 linhas e de 2 a 26 colunas. As colunas são letras de `A` a `Z`.
- As jogadas usam a notação de planilha: `c3` abre, `c3 m` põe ou tira a bandeira.
- Cabeçalho com bombas restantes e cronômetro `mm:ss`. Tela só com ASCII, sem cores, para sair igual em qualquer terminal.
- Gerador No-Guess com até 25000 sorteios. Se nenhum servir, fica valendo o último sorteado.
- O solver usa as mesmas duas regras e **explica cada jogada** (por exemplo `o 1 de B3 ja esta satisfeito`). Quando não há nenhuma certeza, faz o chute de menor risco e mostra a probabilidade estimada.
- O modo automático pode parar a cada jogada e esperar Enter.
- No fim da partida mostra todas as bombas e o tempo total.

### T3: sistema de vendas (`t3-sistema-de-vendas`)

- **Clientes:** código automático, nome, CPF (validado pelos dois dígitos verificadores e sem repetir), telefone e email opcionais. Cadastrar, listar, consultar por código, buscar por parte do nome, alterar campo a campo, remover e ordenar por código ou nome.
- **Produtos:** código automático, nome (sem repetir), preço e estoque, com as mesmas operações e ordenação por código, nome ou preço. O preço aceita vírgula (`12,50`).
- **Vendas:** escolhe o cliente e a data (Enter usa a de hoje) e adiciona produtos até digitar 0. O que já está no carrinho conta como reservado, então não dá para vender mais do que o estoque. O estoque só baixa depois da confirmação.
- Cada item guarda o preço do dia da venda: mudar o preço do produto depois não altera vendas antigas.
- Cliente ou produto que já tem venda não pode ser removido, para não deixar venda apontando para código que não existe.
- A lista de vendas fica sempre em ordem de data (inserção ordenada), então os relatórios por período já saem em ordem cronológica.
- **Relatórios:**
  1. ranking de clientes, por quantidade de compras ou por valor total;
  2. melhores clientes, os N que mais gastaram, com a parte de cada um no faturamento;
  3. produtos mais vendidos por unidades;
  4. vendas por período;
  5. faturamento por período (vendas, unidades, total e ticket médio);
  6. histórico de compras de um cliente, venda por venda com os itens;
  7. vendas por produto (em quantas vendas apareceu, unidades e valor gerado);
  8. produtos com maior faturamento.

  Os rankings usam uma lista auxiliar montada já em ordem.
- **Persistência:** `clientes.txt`, `produtos.txt` e `vendas.txt` na pasta passada como argumento, gravados a cada alteração. Linha estragada é ignorada com aviso e o resto do arquivo continua valendo.
- Toda leitura do teclado é por linha (`fgets` + `sscanf`): letra no lugar de número, número fora do intervalo, data inválida (31/02) e fim da entrada (EOF) são tratados.

## Tecnologias

- C99 com GCC
- Biblioteca padrão (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `ctype.h`)
- `windows.h` (`Sleep`, `SetConsoleOutputCP`) no Windows e `unistd.h` (`usleep`) no Linux, só na versão entregue
- GNU Make

## Estrutura do projeto

| Caminho | Descrição |
|---|---|
| `trabalhos/t1-campo-minado/main.c` | Menu, leitura segura da entrada, modo jogador e modo computador |
| `trabalhos/t1-campo-minado/tabuleiro.c`, `tabuleiro.h` | `Tabuleiro`, alocação e liberação, sorteio das bombas, gerador No-Guess (`Preencher`), flood fill (`Abrir`), bandeiras e impressão |
| `trabalhos/t1-campo-minado/solver.c`, `solver.h` | `ProximoPasso` (as duas regras), `Dica` e `TestaSolucao` (verifica se o tabuleiro se resolve sem chute) |
| `trabalhos/t1-campo-minado/Makefile` | Compilação no Linux e no Windows, com `clean` que funciona no cmd e em shells POSIX |
| `trabalhos/t1-campo-minado/README.md` | Notas técnicas da versão entregue |
| `trabalhos/t1-campo-minado-versao-modular/main.c` | Laço do menu |
| `trabalhos/t1-campo-minado-versao-modular/interface.c`, `.h` | Toda a entrada e saída: menus, leitura das jogadas e desenho |
| `trabalhos/t1-campo-minado-versao-modular/jogo.c`, `.h` | Partida humana e partida do computador, cronômetro |
| `trabalhos/t1-campo-minado-versao-modular/tabuleiro.c`, `.h` | `Celula`, `Tabuleiro`, sorteio, revelação recursiva e marcação |
| `trabalhos/t1-campo-minado-versao-modular/solver.c`, `.h` | Dedução com explicação, chute de menor risco e sorteio resolvível |
| `trabalhos/t3-sistema-de-vendas/main.c` | Menus, cadastro, alteração e remoção, registro de venda |
| `trabalhos/t3-sistema-de-vendas/cliente.c`, `.h` | Lista de clientes, validação do CPF, ordenação por insertion sort nos ponteiros |
| `trabalhos/t3-sistema-de-vendas/produto.c`, `.h` | Lista de produtos e ordenação |
| `trabalhos/t3-sistema-de-vendas/venda.c`, `.h` | Lista de vendas em ordem de data, lista de itens de cada venda, impressão |
| `trabalhos/t3-sistema-de-vendas/arquivo.c`, `.h` | Leitura e gravação dos três arquivos texto |
| `trabalhos/t3-sistema-de-vendas/relatorio.c`, `.h` | Os oito relatórios e a lista auxiliar dos rankings |
| `trabalhos/t3-sistema-de-vendas/util.c`, `.h` | Leitura segura do teclado, datas e funções de texto |
| `trabalhos/t3-sistema-de-vendas/exemplo/` | Dados de exemplo: 6 clientes, 8 produtos e 10 vendas |
| `trabalhos/t3-sistema-de-vendas/Makefile`, `README.md` | Compilação e notas técnicas do T3 |

## Como rodar

### Pré-requisitos

- GCC
  - Linux: `sudo apt install gcc make` (Debian/Ubuntu) ou equivalente
  - Windows: [MSYS2](https://www.msys2.org/) com MinGW-w64 (`pacman -S mingw-w64-ucrt-x86_64-gcc make`), ou WSL
- Make é opcional: dá para compilar direto com `gcc`.

### Passo a passo

```bash
git clone https://github.com/itskelme/ufsm-si-lab-prog-2.git
cd ufsm-si-lab-prog-2
```

**Versão entregue, com Make:**

```bash
cd trabalhos/t1-campo-minado
make                # no MinGW sem MSYS2 o comando pode ser mingw32-make
./campo_minado      # no Windows: .\campo_minado.exe
make clean          # apaga os .o e o executável
```

**Versão entregue, sem Make:**

```bash
cd trabalhos/t1-campo-minado
gcc -Wall -Wextra -std=c99 -O2 -o campo_minado tabuleiro.c solver.c main.c
./campo_minado
```

**Versão modular:**

```bash
cd trabalhos/t1-campo-minado-versao-modular
gcc -Wall -std=c99 -o campominado main.c jogo.c interface.c tabuleiro.c solver.c
./campominado       # no Windows: .\campominado.exe
```

**T3:**

```bash
cd trabalhos/t3-sistema-de-vendas
make                # no Windows: mingw32-make
./vendas exemplo    # no Windows: .\vendas.exe exemplo
```

Sem Make: `gcc -Wall -Wextra -std=c99 -O2 -o vendas util.c cliente.c produto.c venda.c arquivo.c relatorio.c main.c`. O argumento é a pasta dos arquivos de dados. Sem ele, o programa usa a pasta atual e, se os arquivos não existirem, começa vazio.

Em terminais antigos do Windows a moldura da versão entregue do T1 pode aparecer com caracteres estranhos se a fonte não tiver os símbolos de desenho de caixa. O Windows Terminal mostra tudo normalmente. A versão modular e o T3 usam só ASCII.

## Como usar

### Versão entregue

Escolha `1` (jogar) ou `2` (computador) e informe linhas, colunas e bombas. As coordenadas começam em **0** e aparecem nas bordas do tabuleiro.

| Comando | Ação |
|---|---|
| `r L C` | revela a célula da linha `L`, coluna `C` (ex.: `r 2 3`) |
| `b L C` | põe ou tira a bandeira |
| `s` | dica da próxima jogada segura |
| `q` | volta ao menu |

Legenda: `.` fechada, `P` bandeira, número de bombas vizinhas, espaço para vazia e `*` bomba.

```
      0   1   2   3
    ┌───┬───┬───┬───┐
  0 │ . │ . │ 1 │   │
    ├───┼───┼───┼───┤
  1 │ . │ P │ 1 │   │
    └───┴───┴───┴───┘
```

### Versão modular

Escolha `1` (jogar) ou `2` (o computador resolve) e informe linhas (2-30), colunas (2-26) e bombas. A coluna é uma letra e a linha é um número que começa em 1.

| Entrada | Ação |
|---|---|
| `c3` | abre a casa da coluna C, linha 3 |
| `c3 m` | põe ou tira a bandeira em C3 |

Legenda: `.` fechada, `F` bandeira, número, espaço para vazia e `*` bomba. No modo do computador, responda `s` em "Parar a cada jogada" para avançar com Enter.

### T3

O menu principal leva a Clientes, Produtos, Vendas e Relatórios; `0` volta ou sai. Onde o programa pede um código, `0` cancela e `-1` mostra a lista. Na venda, digite os códigos dos produtos e as quantidades e termine com `0`; o programa mostra a venda montada e pede confirmação.

```
+------+--------------------------------+-------+------------+------------+
| Cod  | Produto                        |   Qtd |  Preco un. |   Subtotal |
+------+--------------------------------+-------+------------+------------+
|    1 | Caderno universitario 10 mater |     2 |      24.90 |      49.80 |
|    2 | Caneta esferografica azul      |     5 |       2.50 |      12.50 |
+------+--------------------------------+-------+------------+------------+
                                                     Total: R$      62.30
```

Formato dos arquivos, um registro por linha com campos separados por `;`:

```
clientes.txt   codigo;nome;cpf;telefone;email
produtos.txt   codigo;nome;preco;estoque
vendas.txt     V;codigo;codCliente;dd/mm/aaaa
               I;codProduto;quantidade;precoUnitario   (um por item, abaixo da venda)
```

## Observações

Pontos encontrados ao revisar o código para este README:

- **No-Guess, nas duas versões:** "resolvível" quer dizer resolvível pelas duas regras do solver. Ele não usa a contagem total de bombas nem combina mais de dois números, então alguns tabuleiros que um humano resolveria são descartados. Isso não cria chute, só torna o gerador mais restrito.
- **Versão modular:**
  - Bandeiras colocadas antes da primeira abertura são apagadas, porque a simulação do gerador chama `limparJogadas`.
  - Quando o chute do computador cai em uma bomba, a mensagem final diz "Voce pisou numa bomba".
  - `alocar` não confere o `malloc` de cada linha.
- **T3:**
  - Quando a entrada acaba (EOF), o programa sai com `exit(0)` sem liberar as listas. Os dados já estavam gravados.
  - Buscas por código percorrem a lista inteira, e carregar `vendas.txt` insere cada venda em ordem de data: as duas coisas crescem com o tamanho dos arquivos, o que só pesa com milhares de registros.
  - As tabelas cortam nomes com mais de 30 caracteres, mas o nome inteiro fica gravado e aparece nas telas de detalhe.
  - Nomes com acento são gravados na codificação do terminal (cp850 no console do Windows), então um arquivo de dados editado em UTF-8 pode mostrar acentos errados.

## Referências

- [Minesweeper (video game), Wikipedia](https://en.wikipedia.org/wiki/Minesweeper_(video_game)): regras, primeiro clique seguro com abertura garantida e a discussão de quando o jogo obriga a chutar, que serviram de base para o modo No-Guess
- [Flood fill, Wikipedia](https://en.wikipedia.org/wiki/Flood_fill): abertura das áreas vazias
- Modelo de código e enunciado do T1 fornecidos pelo Prof. Piveta na disciplina
- Enunciado do T3 (sistema de vendas e relatórios) fornecido pelo Prof. Piveta na disciplina
- [Lista ligada, Wikipédia](https://pt.wikipedia.org/wiki/Lista_ligada) e [Insertion sort, Wikipédia](https://pt.wikipedia.org/wiki/Insertion_sort): base das listas e da ordenação do T3
- [Cadastro de Pessoas Físicas, Wikipédia](https://pt.wikipedia.org/wiki/Cadastro_de_Pessoas_F%C3%ADsicas): cálculo dos dígitos verificadores do CPF

## Autores

- Diego Kelme Santos da Silva ([@itskelme](https://github.com/itskelme))
- Prof. Piveta, docente da disciplina, autor do modelo inicial do T1 (`Tabuleiro`, alocar, liberar, preencher, imprimir) e dos enunciados

## Licença

Este projeto está sob a licença [CC BY-NC 4.0](LICENSE): uso pessoal e modificação permitidos com atribuição; uso comercial proibido. Material de terceiros (enunciados, código fornecido pelo professor) pertence aos respectivos autores.
