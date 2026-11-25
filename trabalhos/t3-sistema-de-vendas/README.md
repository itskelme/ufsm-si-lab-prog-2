# Sistema de Vendas (C99 - listas encadeadas)

Cadastro de clientes e produtos, registro de vendas e 8 relatorios. Clientes, produtos, vendas e os itens de
cada venda ficam em listas encadeadas, e tudo e' gravado em arquivo texto a cada alteracao.

## Estrutura do Projeto

- `util.h` e `util.c`: leitura do teclado linha a linha (`fgets` + `sscanf`, trata EOF e lixo), datas e funcoes de texto.
- `cliente.h` e `cliente.c`: lista de clientes, validacao do CPF pelos digitos verificadores e ordenacao por codigo ou nome.
- `produto.h` e `produto.c`: lista de produtos e ordenacao por codigo, nome ou preco.
- `venda.h` e `venda.c`: lista de vendas sempre em ordem de data, cada venda com a sua lista de itens.
- `arquivo.h` e `arquivo.c`: leitura e gravacao de `clientes.txt`, `produtos.txt` e `vendas.txt`.
- `relatorio.h` e `relatorio.c`: os relatorios.
- `main.c`: menus.
- `exemplo/`: 6 clientes, 8 produtos e 10 vendas para testar os relatorios.

## Compilacao e Execucao

```bash
make                  # no Windows: mingw32-make
./vendas exemplo      # no Windows: vendas.exe exemplo
```

Sem `make`:

```bash
gcc -Wall -Wextra -std=c99 -O2 -o vendas util.c cliente.c produto.c venda.c arquivo.c relatorio.c main.c
```

O argumento e' a pasta dos arquivos de dados. Sem ele o programa usa a pasta atual, e se os arquivos nao
existirem comeca vazio.

## Arquivos

Um registro por linha, campos separados por `;` (o `;` digitado num nome vira `,`):

```
clientes.txt   codigo;nome;cpf;telefone;email
produtos.txt   codigo;nome;preco;estoque
vendas.txt     V;codigo;codCliente;dd/mm/aaaa
               I;codProduto;quantidade;precoUnitario      (um por item, logo abaixo da venda)
```

Linha estragada e' ignorada com aviso na tela e o resto do arquivo continua valendo.

## Relatorios

1. Ranking de clientes, por quantidade de compras ou por valor total
2. Melhores clientes: os N que mais gastaram, com a parte de cada um no faturamento
3. Produtos mais vendidos, por unidades
4. Vendas por periodo
5. Faturamento por periodo (vendas, unidades, total e ticket medio)
6. Historico de compras de um cliente, venda por venda
7. Vendas por produto: em quantas vendas apareceu, unidades e valor gerado
8. Produtos com maior faturamento

## Decisoes

- O item da venda guarda o preco do dia: mudar o preco do produto depois nao muda venda antiga.
- Cliente ou produto que ja tem venda nao pode ser removido, senao o historico e os relatorios perdem a referencia.
- O estoque so baixa depois que a venda e' confirmada, e o que ja esta no carrinho conta como reservado.
- Codigo removido nao e' reaproveitado na mesma execucao.
- Ordenar a lista muda a ordem gravada no arquivo (e' a "organizacao" dos dados).
