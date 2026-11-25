#ifndef PRODUTO_H
#define PRODUTO_H

#include "util.h"

#define MAX_PRECO 1000000.0
#define MAX_ESTOQUE 999999

typedef struct Produto {
    int codigo;
    char nome[TAM_NOME];
    double preco;
    int estoque;
    
    
    struct Produto *prox;
} Produto;

typedef struct {
    Produto *inicio;
    int nrProdutos;
    int proximoCodigo;
} ListaProdutos;


ListaProdutos* CriarListaProdutos(void);
void LiberarListaProdutos(ListaProdutos *l);

Produto* NovoProduto(int codigo, const char *nome, double preco, int estoque);
void InserirProduto(ListaProdutos *l, Produto *p);
Produto* BuscarProduto(ListaProdutos *l, int codigo);
Produto* BuscarProdutoPorNome(ListaProdutos *l, const char *nome);
int RemoverProduto(ListaProdutos *l, int codigo);
void OrdenarProdutos(ListaProdutos *l, int criterio);

void ImprimirProduto(Produto *p);
void ImprimirProdutos(ListaProdutos *l, const char *filtroNome);
Produto* EscolherProduto(ListaProdutos *l);

#endif
