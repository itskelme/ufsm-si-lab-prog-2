#include <stdio.h>
#include <stdlib.h>
#include "venda.h"


ListaVendas* CriarListaVendas(void) {
    ListaVendas *l = (ListaVendas*)malloc(sizeof(ListaVendas));
    if (!l) return NULL;

    l->inicio = NULL;
    l->nrVendas = 0;
    l->proximoCodigo = 1;
    return l;
}


void LiberarVenda(Venda *v) {
    if(!v) return;

    ItemVenda *it = v->itens;
    while(it != NULL) {
        ItemVenda *prox = it->prox;
        free(it);
        it = prox;
    }
    free(v);
}


void LiberarListaVendas(ListaVendas *l) {
    if (!l) return;

    Venda *atual = l->inicio;
    while (atual != NULL) {
        Venda *prox = atual->prox;
        LiberarVenda(atual);
        atual = prox;
    }
    free(l);
}


Venda* NovaVenda(int codigo, int codCliente, Data data) {
    Venda *v = (Venda*)malloc(sizeof(Venda));
    if (!v) return NULL;

    v->codigo = codigo;
    v->codCliente = codCliente;
    v->data = data;
    v->itens = NULL;
    v->nrItens = 0;
    v->total = 0.0;
    v->prox = NULL;
    return v;
}


/* Se o mesmo produto ja esta na venda (pelo mesmo preco) so soma a
   quantidade, senao entra um item novo no fim da lista de itens. */
int AdicionarItem(Venda *v, int codProduto, int quantidade, double precoUnitario) {
    ItemVenda *ultimo = NULL;

    for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
        if (it->codProduto == codProduto && it->precoUnitario == precoUnitario) {
            it->quantidade += quantidade;
            v->total = Arredonda(v->total + quantidade * precoUnitario);
            return 1;
        }
        ultimo = it;
    }

    ItemVenda *novo = (ItemVenda*)malloc(sizeof(ItemVenda));
    if (!novo) return 0;

    novo->codProduto = codProduto;
    novo->quantidade = quantidade;
    novo->precoUnitario = precoUnitario;
    novo->prox = NULL;

    if (ultimo == NULL) v->itens = novo;
    else ultimo->prox = novo;

    v->nrItens++;
    v->total = Arredonda(v->total + quantidade * precoUnitario);
    return 1;
}


int QuantidadeNaVenda(Venda *v, int codProduto) {
    int qtd = 0;
    for(ItemVenda *it = v->itens; it != NULL; it = it->prox) {
        if (it->codProduto == codProduto) qtd += it->quantidade;
    }
    return qtd;
}


/* A lista de vendas fica sempre em ordem de data, assim os relatorios por
   periodo ja saem em ordem cronologica. Venda com a mesma data de outra
   entra depois dela. */
void InserirVenda(ListaVendas *l, Venda *v) {
    int chave = DataParaInt(v->data);

    if (l->inicio == NULL || chave < DataParaInt(l->inicio->data)) {
        v->prox = l->inicio;
        l->inicio = v;
    } else {
        Venda *p = l->inicio;
        while (p->prox != NULL && DataParaInt(p->prox->data) <= chave) {
            p = p->prox;
        }
        v->prox = p->prox;
        p->prox = v;
    }

    l->nrVendas++;
    if (v->codigo >= l->proximoCodigo) l->proximoCodigo = v->codigo + 1;
}


Venda* BuscarVenda(ListaVendas *l, int codigo) {
    for (Venda *v = l->inicio; v != NULL; v = v->prox) {
        if (v->codigo == codigo) return v;
    }
    return NULL;
}


int ClienteTemVendas(ListaVendas *l, int codCliente) {
    for (Venda *v = l->inicio; v != NULL; v = v->prox) {
        if (v->codCliente == codCliente) return 1;
    }
    return 0;
}


int ProdutoTemVendas(ListaVendas *l, int codProduto) {
    for (Venda *v = l->inicio; v != NULL; v = v->prox) {
        if (QuantidadeNaVenda(v, codProduto) > 0) return 1;
    }
    return 0;
}


static void LinhaItens(void) {
    printf("+------+--------------------------------+-------+------------+------------+\n");
}


void ImprimirVenda(Venda *v, ListaClientes *clientes, ListaProdutos *produtos) {
    Cliente *c = BuscarCliente(clientes, v->codCliente);
    
    printf("\nVenda %d - %02d/%02d/%04d\n", v->codigo, v->data.dia, v->data.mes, v->data.ano);
    printf("Cliente: %d - %s\n", v->codCliente, c ? c->nome : "(cliente nao encontrado)");

    LinhaItens();
    printf("| %-4s | %-30s | %5s | %10s | %10s |\n", "Cod", "Produto", "Qtd", "Preco un.", "Subtotal");
    LinhaItens();

    for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
        Produto *p = BuscarProduto(produtos, it->codProduto);
        printf("| %4d | %-30.30s | %5d | %10.2f | %10.2f |\n",
               it->codProduto, p ? p->nome : "(produto nao encontrado)",
               it->quantidade, it->precoUnitario, it->quantidade * it->precoUnitario);
    }

    LinhaItens();
    printf("%59s R$ %10.2f\n", "Total:", v->total);
}


static void LinhaVendas(void) {
    printf("+--------+------------+--------------------------------+-------+------------+\n");
}


/* Uma linha por venda, so das que estao entre as datas de e ate (no formato
   do DataParaInt). Para listar tudo e' so passar 0 e 99999999. */
void ImprimirVendas(ListaVendas *l, ListaClientes *clientes, int de, int ate) {
    int qtd = 0;
    double soma = 0.0;

    if (l->nrVendas == 0) {
        printf("\nNenhuma venda registrada ainda, o caixa ta vazio.\n");
        return;
    }

    printf("\n");
    LinhaVendas();
    printf("| %-6s | %-10s | %-30s | %5s | %10s |\n", "Venda", "Data", "Cliente", "Itens", "Total R$");
    LinhaVendas();

    for (Venda *v = l->inicio; v != NULL; v = v->prox) {
        int d = DataParaInt(v->data);
        if (d < de || d > ate) continue;

        Cliente *c = BuscarCliente(clientes, v->codCliente);
        printf("| %6d | %02d/%02d/%04d | %-30.30s | %5d | %10.2f |\n",
               v->codigo, v->data.dia, v->data.mes, v->data.ano,
               c ? c->nome : "(cliente nao encontrado)", v->nrItens, v->total);
        qtd++;
        soma += v->total;
    }

    LinhaVendas();
    printf("%d venda(s), somando R$ %.2f\n", qtd, soma);
}
