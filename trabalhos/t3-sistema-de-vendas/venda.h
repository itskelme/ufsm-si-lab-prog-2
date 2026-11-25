#ifndef VENDA_H
#define VENDA_H

#include "util.h"
#include "cliente.h"
#include "produto.h"

/* Cada venda tem a sua propria lista de itens. O item guarda o preco do dia
   da venda: se o produto mudar de preco depois, a venda antiga nao muda. */
typedef struct ItemVenda {
    int codProduto;
    int quantidade;
    double precoUnitario;
    struct ItemVenda *prox;
} ItemVenda;

typedef struct Venda {
    int codigo;
    
    
    int codCliente;
    Data data;
    ItemVenda *itens;
    int nrItens;
    double total;
    struct Venda *prox;
} Venda;

typedef struct {
    Venda *inicio;
    int nrVendas;
    int proximoCodigo;
} ListaVendas;


ListaVendas* CriarListaVendas(void);
void LiberarListaVendas(ListaVendas *l);

Venda* NovaVenda(int codigo, int codCliente, Data data);
void LiberarVenda(Venda *v);
int AdicionarItem(Venda *v, int codProduto, int quantidade, double precoUnitario);
int QuantidadeNaVenda(Venda *v, int codProduto);

void InserirVenda(ListaVendas *l, Venda *v);
Venda* BuscarVenda(ListaVendas *l, int codigo);
int ClienteTemVendas(ListaVendas *l, int codCliente);
int ProdutoTemVendas(ListaVendas *l, int codProduto);

void ImprimirVenda(Venda *v, ListaClientes *clientes, ListaProdutos *produtos);
void ImprimirVendas(ListaVendas *l, ListaClientes *clientes, int de, int ate);

#endif
