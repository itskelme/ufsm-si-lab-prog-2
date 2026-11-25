#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "produto.h"


ListaProdutos* CriarListaProdutos(void) {
    ListaProdutos *l = (ListaProdutos*)malloc(sizeof(ListaProdutos));
    if (!l) return NULL;

    l->inicio = NULL;
    l->nrProdutos = 0;
    l->proximoCodigo = 1;
    return l;
}


void LiberarListaProdutos(ListaProdutos *l) {
    if(!l) return;

    Produto *atual = l->inicio;
    while(atual != NULL) {
        Produto *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(l);
}


Produto* NovoProduto(int codigo, const char *nome, double preco, int estoque) {
    Produto *p = (Produto*)malloc(sizeof(Produto));
    if (!p) return NULL;

    p->codigo = codigo;
    CopiaTexto(p->nome, nome, TAM_NOME);
    p->preco = preco;
    p->estoque = estoque;
    p->prox = NULL;
    return p;
}


void InserirProduto(ListaProdutos *l, Produto *p) {
    p->prox = NULL;

    if (l->inicio == NULL) {
        l->inicio = p;
    } else {
        Produto *q = l->inicio;
        while (q->prox != NULL) q = q->prox;
        q->prox = p;
    }

    l->nrProdutos++;
    if (p->codigo >= l->proximoCodigo) l->proximoCodigo = p->codigo + 1;
}


Produto* BuscarProduto(ListaProdutos *l, int codigo) {
    for (Produto *p = l->inicio; p != NULL; p = p->prox) {
        if (p->codigo == codigo) return p;
    }
    return NULL;
}


// usado so pra nao deixar cadastrar dois produtos com o mesmo nome
Produto* BuscarProdutoPorNome(ListaProdutos *l, const char *nome) {
    for (Produto *p = l->inicio; p != NULL; p = p->prox) {
        if (ComparaSemCaixa(p->nome, nome) == 0) return p;
    }
    return NULL;
}


int RemoverProduto(ListaProdutos *l, int codigo) {
    Produto *ant = NULL;
    Produto *p = l->inicio;

    while (p != NULL && p->codigo != codigo) {
        ant = p;
        p = p->prox;
    }
    if (p == NULL) return 0;

    if (ant == NULL) l->inicio = p->prox;
    else ant->prox = p->prox;

    free(p);
    l->nrProdutos--;
    return 1;
}

static int ProdutoVemAntes(Produto *a, Produto *b, int criterio) {
    if (criterio == ORDEM_NOME) {
        int cmp = ComparaSemCaixa(a->nome, b->nome);
        if (cmp != 0) return cmp < 0;
    } else if (criterio == ORDEM_PRECO) {
        if (a->preco != b->preco) return a->preco < b->preco;
    }
    return a->codigo < b->codigo;
}


/* mesma ideia do OrdenarClientes: insertion sort trocando so os ponteiros */
void OrdenarProdutos(ListaProdutos *l, int criterio) {
    Produto *ordenada = NULL;
    Produto *atual = l->inicio;

    while (atual != NULL) {
        Produto *prox = atual->prox;

        if (ordenada == NULL || ProdutoVemAntes(atual, ordenada, criterio)) {
            atual->prox = ordenada;
            ordenada = atual;
        } else {
            Produto *p = ordenada;
            while (p->prox != NULL && !ProdutoVemAntes(atual, p->prox, criterio)) {
                p = p->prox;
            }
            atual->prox = p->prox;
            p->prox = atual;
        }
        atual = prox;
    }

    l->inicio = ordenada;
}


void ImprimirProduto(Produto *p) {
    printf("\nCodigo:  %d\n", p->codigo);
    printf("Nome:    %s\n", p->nome);
    printf("Preco:   R$ %.2f\n", p->preco);
    printf("Estoque: %d\n", p->estoque);
}


static void LinhaProdutos(void) {
    printf("+------+--------------------------------+------------+---------+\n"); 
}



void ImprimirProdutos(ListaProdutos *l, const char *filtroNome) {
    int mostrados = 0;

    if (l->nrProdutos == 0) {  
        printf("\nNenhum produto cadastrado ainda.\n");
        return;
    }

    printf("\n");
    LinhaProdutos();
    printf("| %-4s | %-30s | %10s | %7s |\n", "Cod", "Produto", "Preco R$", "Estoque");
    LinhaProdutos();

    for (Produto *p = l->inicio; p != NULL; p = p->prox) {
        if (filtroNome != NULL && !ContemTexto(p->nome, filtroNome)) continue;
        printf("| %4d | %-30.30s | %10.2f | %7d |\n", p->codigo, p->nome, p->preco, p->estoque);
        mostrados++;
    }

    LinhaProdutos();
    printf("%d produto(s)\n", mostrados);
}


Produto* EscolherProduto(ListaProdutos *l) {
    if (l->nrProdutos == 0) {
        printf("\nNenhum produto cadastrado ainda.\n");
        return NULL;
    }

    while (1) {
        int cod = LerInteiro("Codigo do produto (0 cancela, -1 lista)", -1, MAX_CODIGO);
        if (cod == 0) return NULL;
        if (cod == -1) {
            ImprimirProdutos(l, NULL);
            continue;
        }

        Produto *p = BuscarProduto(l, cod);
        if (p != NULL) return p;
        printf("Nao existe produto com o codigo %d.\n", cod);
    }
}
