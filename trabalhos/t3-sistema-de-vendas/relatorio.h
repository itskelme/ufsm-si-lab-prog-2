#ifndef RELATORIO_H
#define RELATORIO_H

#include "cliente.h"
#include "produto.h"
#include "venda.h"

void RelatorioRankingClientes(ListaClientes *clientes, ListaVendas *vendas);
void RelatorioMelhoresClientes(ListaClientes *clientes, ListaVendas *vendas);
void RelatorioProdutosMaisVendidos(ListaProdutos *produtos, ListaVendas *vendas);
void RelatorioVendasPeriodo(ListaClientes *clientes, ListaVendas *vendas);
void RelatorioFaturamentoPeriodo(ListaVendas *vendas);
void RelatorioHistoricoCliente(ListaClientes *clientes, ListaProdutos *produtos, ListaVendas *vendas);
void RelatorioVendasPorProduto(ListaProdutos *produtos, ListaVendas *vendas);
void RelatorioProdutosFaturamento(ListaProdutos *produtos, ListaVendas *vendas);

#endif
