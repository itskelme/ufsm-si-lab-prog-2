#include <stdio.h>
#include <stdlib.h>
#include "relatorio.h"

#define POR_QUANTIDADE 1
#define POR_VALOR      2


/* Lista auxiliar dos rankings: cada posicao e' um cliente ou um produto com
   os totais dele. Ela ja e' montada em ordem (insercao ordenada, do maior
   pro menor), entao no fim e' so imprimir os N primeiros e liberar. */
typedef struct Posicao {
    int codigo;
    int quantidade;     /* compras do cliente ou unidades vendidas do produto */
    double valor;
    struct Posicao *prox;
} Posicao;


static int PassaNaFrente(Posicao *a, Posicao *b, int criterio) {
    if(criterio == POR_QUANTIDADE) {
        if(a->quantidade != b->quantidade) return a->quantidade > b->quantidade;
        return a->valor > b->valor;     // empate na quantidade: desempata pelo valor
    }
    if(a->valor != b->valor) return a->valor > b->valor;
    return a->quantidade > b->quantidade;
}


/* Empate completo fica com quem entrou primeiro */
static Posicao* InserirNoRanking(Posicao *inicio, int codigo, int quantidade, double valor, int criterio) {
    Posicao *novo = (Posicao*)malloc(sizeof(Posicao));
    if (!novo) return inicio;

    novo->codigo = codigo;
    novo->quantidade = quantidade;
    novo->valor = valor;

    if (inicio == NULL || PassaNaFrente(novo, inicio, criterio)) {
        novo->prox = inicio;
        return novo;
    }

    Posicao *p = inicio;
    while (p->prox != NULL && !PassaNaFrente(novo, p->prox, criterio)) {
        p = p->prox;
    }
    novo->prox = p->prox;
    p->prox = novo;
    return inicio;
}


static void LiberarRanking(Posicao *p) {
    while (p != NULL) {
        Posicao *prox = p->prox;
        free(p);
        p = prox;
    }
}


static void TotalCliente(ListaVendas *vendas, int codCliente, int *compras, double *valor) {
    *compras = 0;
    *valor = 0.0;

    for (Venda *v = vendas->inicio; v != NULL; v = v->prox) {
        if (v->codCliente == codCliente) {
            (*compras)++;
            *valor += v->total;
        }
    }
}


/* vezes = em quantas vendas o produto apareceu, unidades = quantas unidades
   sairam no total, valor = quanto essas unidades renderam */
static void TotalProduto(ListaVendas *vendas, int codProduto, int *vezes, int *unidades, double *valor) {
    *vezes = 0;
    *unidades = 0;
    *valor = 0.0;

    for (Venda *v = vendas->inicio; v != NULL; v = v->prox) {
        int apareceu = 0;
        for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
            if (it->codProduto == codProduto) {
                *unidades += it->quantidade;
                *valor += it->quantidade * it->precoUnitario;
                apareceu = 1;
            }
        }
        if (apareceu) (*vezes)++;
    }
}


static double FaturamentoTotal(ListaVendas *vendas) {
    double total = 0.0;
    for(Venda *v = vendas->inicio; v != NULL; v = v->prox) {
        total += v->total;
    }
    return total;
}


// so entra no ranking quem comprou pelo menos uma vez
static Posicao* RankingClientes(ListaClientes *clientes, ListaVendas *vendas, int criterio) {
    Posicao *ranking = NULL;

    for (Cliente *c = clientes->inicio; c != NULL; c = c->prox) {
        int compras;
        double valor;
        TotalCliente(vendas, c->codigo, &compras, &valor);
        if (compras > 0) ranking = InserirNoRanking(ranking, c->codigo, compras, valor, criterio);
    }
    return ranking;
}


static Posicao* RankingProdutos(ListaProdutos *produtos, ListaVendas *vendas, int criterio) {
    Posicao *ranking = NULL;

    for (Produto *p = produtos->inicio; p != NULL; p = p->prox) {
        int vezes, unidades;
        double valor;
        TotalProduto(vendas, p->codigo, &vezes, &unidades, &valor);
        if (unidades > 0) ranking = InserirNoRanking(ranking, p->codigo, unidades, valor, criterio);	
    }
    return ranking;
}


static double Percentual(double parte, double total) {
    if (total <= 0.0) return 0.0;
    return parte * 100.0 / total;
}


static void LinhaRanking(void) {
    printf("+-----+------+--------------------------------+----------+--------------+---------+\n");
}


/* Imprime as n primeiras posicoes. Serve pros quatro rankings, so muda o
   nome da coluna de quantidade e de onde vem o nome (cliente ou produto). */
static void ImprimirRanking(Posicao *ranking, int n, const char *colQtd,
                            ListaClientes *clientes, ListaProdutos *produtos, double faturamento) {
    int pos = 0;
    
    printf("\n");
    LinhaRanking();
    printf("| %-3s | %-4s | %-30s | %8s | %12s | %7s |\n", "Pos", "Cod", "Nome", colQtd, "Total R$", "% fat.");
    LinhaRanking();

    for (Posicao *p = ranking; p != NULL && pos < n; p = p->prox) {
        const char *nome = "?";
        if (clientes != NULL) {
            Cliente *c = BuscarCliente(clientes, p->codigo);
            if (c) nome = c->nome;
        } else {
            Produto *pr = BuscarProduto(produtos, p->codigo);
            if (pr) nome = pr->nome;
        }

        pos++;
        printf("| %3d | %4d | %-30.30s | %8d | %12.2f | %6.1f%% |\n",
               pos, p->codigo, nome, p->quantidade, p->valor, Percentual(p->valor, faturamento));
    }

    LinhaRanking();
    if (pos < n) printf("(so %d na lista, os outros ainda nao tem venda)\n", pos);
}



static int SemVendas(ListaVendas *vendas) {
    if (vendas->nrVendas == 0) {
        printf("Nenhuma venda registrada ainda, nao tem o que mostrar.\n");
        return 1;
    }
    return 0;
}


/* 1. Ranking de clientes: todos que compraram, por quantidade de compras ou
      por valor total, a escolha de quem pediu */
void RelatorioRankingClientes(ListaClientes *clientes, ListaVendas *vendas) {
    printf("\n-- Ranking de clientes --\n"); 
    if(SemVendas(vendas)) return;

    printf("1. Por quantidade de compras\n");
    printf("2. Por valor total em compras\n");
    int criterio = LerInteiro("Criterio", 1, 2);

    Posicao *ranking = RankingClientes(clientes, vendas, criterio == 1 ? POR_QUANTIDADE : POR_VALOR);
    ImprimirRanking(ranking, clientes->nrClientes, "Compras", clientes, NULL, FaturamentoTotal(vendas));
    LiberarRanking(ranking);
}


/* 2. Melhores clientes: os N que mais gastaram */
void RelatorioMelhoresClientes(ListaClientes *clientes, ListaVendas *vendas) {
    printf("\n-- Melhores clientes (maior valor em compras) --\n");
    if (SemVendas(vendas) || clientes->nrClientes == 0) return;

    int n = LerInteiro("Quantos clientes mostrar", 1, clientes->nrClientes);

    Posicao *ranking = RankingClientes(clientes, vendas, POR_VALOR);
    ImprimirRanking(ranking, n, "Compras", clientes, NULL, FaturamentoTotal(vendas));
    LiberarRanking(ranking);
}

/* 3. Produtos mais vendidos: por unidades */
void RelatorioProdutosMaisVendidos(ListaProdutos *produtos, ListaVendas *vendas) {
    printf("\n-- Produtos mais vendidos (unidades) --\n");
    if (SemVendas(vendas) || produtos->nrProdutos == 0) return;

    int n = LerInteiro("Quantos produtos mostrar", 1, produtos->nrProdutos);

    Posicao *ranking = RankingProdutos(produtos, vendas, POR_QUANTIDADE);
    ImprimirRanking(ranking, n, "Unidades", NULL, produtos, FaturamentoTotal(vendas));
    LiberarRanking(ranking);
}


static void LerPeriodo(Data *ini, Data *fim) {
    *ini = LerData("Data inicial", 0);
    *fim = LerData("Data final", 0);

    if (DataParaInt(*ini) > DataParaInt(*fim)) {
        Data aux = *ini;
        *ini = *fim;
        *fim = aux;
        printf("A data inicial era depois da final, troquei as duas.\n");
    }
}



/* 4. Vendas por periodo */
void RelatorioVendasPeriodo(ListaClientes *clientes, ListaVendas *vendas) {
    Data ini, fim;

    printf("\n-- Vendas por periodo --\n");
    if (SemVendas(vendas)) return;

    LerPeriodo(&ini, &fim);
    printf("\nVendas de %02d/%02d/%04d a %02d/%02d/%04d:\n",
           ini.dia, ini.mes, ini.ano, fim.dia, fim.mes, fim.ano);
    ImprimirVendas(vendas, clientes, DataParaInt(ini), DataParaInt(fim));
}


/* 5. Faturamento por periodo */  
void RelatorioFaturamentoPeriodo(ListaVendas *vendas) {
    Data ini, fim;
    int qtd = 0, unidades = 0;
    double total = 0.0;

    printf("\n-- Faturamento por periodo --\n");
    if (SemVendas(vendas)) return;

    LerPeriodo(&ini, &fim);
    int de = DataParaInt(ini);
    int ate = DataParaInt(fim);

    for (Venda *v = vendas->inicio; v != NULL; v = v->prox) {
        int d = DataParaInt(v->data);
        if (d < de || d > ate) continue;

        qtd++;
        total += v->total;
        for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
            unidades += it->quantidade;
        }
    }

    printf("\nPeriodo:           %02d/%02d/%04d a %02d/%02d/%04d\n",
           ini.dia, ini.mes, ini.ano, fim.dia, fim.mes, fim.ano);
    printf("Vendas:            %d\n", qtd);
    printf("Unidades vendidas: %d\n", unidades);
    printf("Faturamento:       R$ %.2f\n", total);
    if (qtd > 0) printf("Ticket medio:      R$ %.2f\n", total / qtd);
}


/* 6. Historico de compras de um cliente, venda por venda com os itens */
void RelatorioHistoricoCliente(ListaClientes *clientes, ListaProdutos *produtos, ListaVendas *vendas) {
    printf("\n-- Historico de compras do cliente --\n");

    Cliente *c = EscolherCliente(clientes);
    if(!c) return;

    int compras = 0;
    double total = 0.0;
    Venda *primeira = NULL;
    Venda *ultima = NULL;

    printf("\nHistorico de %s:\n", c->nome);
    for(Venda *v = vendas->inicio; v != NULL; v = v->prox) {
        if(v->codCliente != c->codigo)  continue;

        ImprimirVenda(v, clientes, produtos);
        compras++;
        total += v->total;
        if (primeira == NULL) primeira = v;
        ultima = v;     // a lista esta em ordem de data
    }

    if(compras == 0) {
        printf("%s ainda nao comprou nada.\n", c->nome);
        return;
    }

    printf("\nResumo: %d compra(s), R$ %.2f no total, ticket medio de R$ %.2f\n",
           compras, total, total / compras);
    printf("Primeira compra em %02d/%02d/%04d, ultima em %02d/%02d/%04d\n",
           primeira->data.dia, primeira->data.mes, primeira->data.ano,
           ultima->data.dia, ultima->data.mes, ultima->data.ano);
}



static void LinhaVendasProduto(void) {
    printf("+------+--------------------------------+-------+----------+--------------+\n");
}



/* 7. Vendas por produto: quantas vezes cada produto foi vendido e quanto
      rendeu. Codigo 0 mostra todos os produtos */
void RelatorioVendasPorProduto(ListaProdutos *produtos, ListaVendas *vendas) {
    printf("\n-- Vendas por produto --\n");
    if (produtos->nrProdutos == 0) {
        printf("Nenhum produto cadastrado ainda.\n");
        return;
    }

    int cod = LerInteiro("Codigo do produto (0 = todos)", 0, MAX_CODIGO);
    if (cod != 0 && BuscarProduto(produtos, cod) == NULL) {
        printf("Nao existe produto com o codigo %d.\n", cod);
        return;
    }

    printf("\n");
    LinhaVendasProduto();
    printf("| %-4s | %-30s | %5s | %8s | %12s |\n", "Cod", "Produto", "Vezes", "Unidades", "Valor R$");
    LinhaVendasProduto();

    for (Produto *p = produtos->inicio; p != NULL; p = p->prox) {
        if (cod != 0 && p->codigo != cod) continue;

        int vezes, unidades;
        double valor;
        TotalProduto(vendas, p->codigo, &vezes, &unidades, &valor);
        printf("| %4d | %-30.30s | %5d | %8d | %12.2f |\n", p->codigo, p->nome, vezes, unidades, valor);
    }

    LinhaVendasProduto();
    printf("Vezes = em quantas vendas o produto apareceu\n");
}


/* 8. Produtos que mais faturaram */
void RelatorioProdutosFaturamento(ListaProdutos *produtos, ListaVendas *vendas) {
    printf("\n-- Produtos com maior faturamento --\n");
    if (SemVendas(vendas) || produtos->nrProdutos == 0) return;

    int n = LerInteiro("Quantos produtos mostrar", 1, produtos->nrProdutos);

    Posicao *ranking = RankingProdutos(produtos, vendas, POR_VALOR);
    ImprimirRanking(ranking, n, "Unidades", NULL, produtos, FaturamentoTotal(vendas));
    LiberarRanking(ranking);
}
