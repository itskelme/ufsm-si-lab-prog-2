#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "cliente.h"
#include "produto.h"
#include "venda.h"

#define ARQ_CLIENTES "clientes.txt"
#define ARQ_PRODUTOS "produtos.txt"
#define ARQ_VENDAS   "vendas.txt"

/* Carregar devolve quantos registros foram lidos, ou -1 se o arquivo ainda
   nao existe (primeira vez que o programa roda naquela pasta).
   Salvar devolve 1 se gravou e 0 se deu erro. */
int CarregarClientes(ListaClientes *l, const char *caminho);
int CarregarProdutos(ListaProdutos *l, const char *caminho);
int CarregarVendas(ListaVendas *l, const char *caminho);

int SalvarClientes(ListaClientes *l, const char *caminho);
int SalvarProdutos(ListaProdutos *l, const char *caminho);
int SalvarVendas(ListaVendas *l, const char *caminho);


#endif
