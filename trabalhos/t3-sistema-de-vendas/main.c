#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "cliente.h"
#include "produto.h"
#include "venda.h"
#include "arquivo.h"
#include "relatorio.h"

// Trabalho 3 - Laboratorio de Programacao 2 - Diego Kelme
// Sistema de vendas: cadastro de clientes e produtos, registro das vendas e
// relatorios. Clientes,  produtos, vendas e os itens de cada venda ficam em
// listas encadeadas, e tudo e' gravado em arquivo texto a cada alteracao.
//
// compila com: make   (ou gcc -Wall -Wextra -std=c99 -o vendas *.c)
// executa com: ./vendas [pasta dos arquivos]     (sem pasta usa a atual)


/* tudo que o programa usa junto, declarado na main e passado por ponteiro */
typedef struct {
    ListaClientes *clientes;
    ListaProdutos *produtos;
    ListaVendas *vendas;
    char arqClientes[TAM_LINHA];
    char arqProdutos[TAM_LINHA];
    char arqVendas[TAM_LINHA];
} Sistema;


/* Le o CPF, tira pontos e traco e confere. Nao deixa repetir CPF de outro
   cliente (o proprio dono pode, pra quando esta alterando).
   Enter vazio desiste e devolve 0. */
static int LerCpf(Sistema *s, char *cpf, int codigoDono) {
    char digitado[TAM_LINHA];

    while (1) {
        if (LerTexto("CPF (Enter desiste)", digitado, TAM_LINHA, 0) == 0) return 0;

        int n = SoDigitos(digitado, cpf, TAM_CPF);
        if (n != 11 || !CpfValido(cpf)) {
            printf("CPF invalido, confere os numeros ai.\n");
            continue;
        }

        Cliente *outro = BuscarClientePorCpf(s->clientes, cpf);
        if (outro != NULL && outro->codigo != codigoDono) {
            printf("Esse CPF ja e' do cliente %d (%s).\n", outro->codigo, outro->nome);
            continue;
        }
        return 1;
    }
}


static void CadastrarCliente(Sistema *s) {
    char nome[TAM_NOME], cpf[TAM_CPF], telefone[TAM_TELEFONE], email[TAM_EMAIL];

    printf("\n-- Novo cliente --\n");
    LerTexto("Nome", nome, TAM_NOME, 1);
    if (!LerCpf(s, cpf, 0)) {
        printf("Cadastro cancelado.\n");
        return;
    }
    LerTexto("Telefone (opcional)", telefone, TAM_TELEFONE, 0);
    LerTexto("Email (opcional)", email, TAM_EMAIL, 0);

    Cliente *c = NovoCliente(s->clientes->proximoCodigo, nome, cpf, telefone, email);
    if(!c) {
        printf("Erro ao alocar memoria para o cliente.\n");
        return;
    }

    InserirCliente(s->clientes, c);
    SalvarClientes(s->clientes, s->arqClientes);
    printf("Cliente cadastrado com o codigo %d.\n", c->codigo);
}


static void AlterarCliente(Sistema *s) {
    printf("\n-- Alterar cliente --\n");
    Cliente *c = EscolherCliente(s->clientes);
    if(!c) return;

    int opcao;
    do {
        ImprimirCliente(c);
        printf("\nO que alterar? 1-Nome  2-CPF  3-Telefone  4-Email  0-Terminar\n");
        opcao = LerInteiro("Opcao", 0, 4);

        if (opcao == 1) {
            LerTexto("Novo nome", c->nome, TAM_NOME, 1);
        } else if(opcao == 2) {
            char cpf[TAM_CPF];
            if(LerCpf(s, cpf, c->codigo)) strcpy(c->cpf, cpf);
        } else if(opcao == 3) {
            LerTexto("Novo telefone (Enter apaga)", c->telefone, TAM_TELEFONE, 0);
        } else if (opcao == 4) {
            LerTexto("Novo email (Enter apaga)", c->email, TAM_EMAIL, 0);
        }

        if(opcao != 0) SalvarClientes(s->clientes, s->arqClientes);
    } while(opcao != 0);
}


static void RemoverClienteMenu(Sistema *s) {
    printf("\n-- Remover cliente --\n");
    Cliente *c = EscolherCliente(s->clientes);
    if (!c) return;

    /* se apagar um cliente que ja comprou, as vendas dele ficam apontando pra
       um codigo que nao existe mais e o historico se perde */
    if(ClienteTemVendas(s->vendas, c->codigo)) {
        printf("%s tem vendas registradas, nao da pra remover sem perder o historico.\n", c->nome);
        return;
    }

    ImprimirCliente(c);
    if (!Confirmar("Remover esse cliente?")) return;

    RemoverCliente(s->clientes, c->codigo);
    SalvarClientes(s->clientes, s->arqClientes);
    printf("Cliente removido.\n");
}


static void MenuClientes(Sistema *s) {
    int opcao;

    do {
        printf("\n--- CLIENTES ---\n");
        printf("1. Cadastrar\n");
        printf("2. Listar todos\n");
        printf("3. Consultar por codigo\n");
        printf("4. Buscar por nome\n");
        printf("5. Alterar\n");
        printf("6. Remover\n");
        printf("7. Ordenar a lista\n");
        printf("0. Voltar\n");
        opcao = LerInteiro("Opcao", 0, 7);

        switch (opcao) {
            case 1:
                CadastrarCliente(s);
                break;
            case 2:
                ImprimirClientes(s->clientes, NULL);
                break;
            case 3: {
                Cliente *c = EscolherCliente(s->clientes);
                if (c) ImprimirCliente(c);
                break;
            }
            case 4: {
                char trecho[TAM_NOME];
                
                
                LerTexto("Parte do nome", trecho, TAM_NOME, 1);
                ImprimirClientes(s->clientes, trecho);
                break;
            }
            case 5:
                AlterarCliente(s);
                break;
            case 6:
                RemoverClienteMenu(s);
                break;
            case 7: {
                int criterio = LerInteiro("Ordenar por 1-codigo  2-nome", 1, 2);
                OrdenarClientes(s->clientes, criterio == 1 ? ORDEM_CODIGO : ORDEM_NOME);
                SalvarClientes(s->clientes, s->arqClientes);
                ImprimirClientes(s->clientes, NULL);
                break;
            }
        }
    } while (opcao != 0);
}


static void LerNomeProduto(Sistema *s, char *nome, int codigoDono) {
    while (1) {
        LerTexto("Nome do produto", nome, TAM_NOME, 1);
        Produto *outro = BuscarProdutoPorNome(s->produtos, nome);
        if (outro == NULL || outro->codigo == codigoDono) return;
        printf("Ja tem um produto com esse nome (codigo %d).\n", outro->codigo);
    }
}


static void CadastrarProduto(Sistema *s) {
    char nome[TAM_NOME];

    printf("\n-- Novo produto --\n");
    LerNomeProduto(s, nome, 0);
    double preco = Arredonda(LerReal("Preco (R$)", 0.01, MAX_PRECO));
    int estoque = LerInteiro("Estoque inicial", 0, MAX_ESTOQUE);

    Produto *p = NovoProduto(s->produtos->proximoCodigo, nome, preco, estoque);
    if (!p) {
        printf("Erro ao alocar memoria para o produto.\n");
        return;
    }

    InserirProduto(s->produtos, p);
    SalvarProdutos(s->produtos, s->arqProdutos);
    printf("Produto cadastrado com o codigo %d.\n", p->codigo);
}


static void AlterarProduto(Sistema *s) {
    printf("\n-- Alterar produto --\n");
    Produto *p = EscolherProduto(s->produtos);
    if (!p) return;

    int opcao;
    do {
        ImprimirProduto(p);
        printf("\nO que alterar? 1-Nome  2-Preco  3-Estoque  0-Terminar\n");
        opcao = LerInteiro("Opcao", 0, 3);

        if (opcao == 1) {
            char nome[TAM_NOME];
            LerNomeProduto(s, nome, p->codigo);
            strcpy(p->nome, nome);
        } else if (opcao == 2) {
            // as vendas antigas guardaram o preco delas, entao nao mudam
            p->preco = Arredonda(LerReal("Novo preco (R$)", 0.01, MAX_PRECO));
        } else if (opcao == 3) {
            p->estoque = LerInteiro("Estoque atual", 0, MAX_ESTOQUE);
        }

        if (opcao != 0) SalvarProdutos(s->produtos, s->arqProdutos);
    } while (opcao != 0);
}


static void RemoverProdutoMenu(Sistema *s) {
    printf("\n-- Remover produto --\n");
    Produto *p = EscolherProduto(s->produtos);
    if (!p) return;

    if (ProdutoTemVendas(s->vendas, p->codigo)) {
        printf("%s ja foi vendido, se sair da lista os relatorios ficam furados.\n", p->nome);
        printf("Se nao vende mais, deixa o estoque em 0.\n");
        return;
    }

    ImprimirProduto(p);
    if (!Confirmar("Remover esse produto?")) return;

    RemoverProduto(s->produtos, p->codigo);
    SalvarProdutos(s->produtos, s->arqProdutos);
    printf("Produto removido.\n");
}


static void MenuProdutos(Sistema *s) {
    int opcao;

    do {
        printf("\n--- PRODUTOS ---\n");
        printf("1. Cadastrar\n");
        printf("2. Listar todos\n");
        printf("3. Consultar por codigo\n");
        printf("4. Buscar por nome\n");
        printf("5. Alterar\n");
        printf("6. Remover\n");
        printf("7. Ordenar a lista\n");
        printf("0. Voltar\n");
        opcao = LerInteiro("Opcao", 0, 7);

        switch (opcao) {
            case 1:
                CadastrarProduto(s);
                break;
            case 2:
                ImprimirProdutos(s->produtos, NULL);
                break;
            case 3: {
                Produto *p = EscolherProduto(s->produtos);
                if (p) ImprimirProduto(p);
                break;
            }
            case 4: {
                char trecho[TAM_NOME];
                LerTexto("Parte do nome", trecho, TAM_NOME, 1);
                ImprimirProdutos(s->produtos, trecho);
                break;
            }
            case 5:
                AlterarProduto(s);
                break;
            case 6:
                RemoverProdutoMenu(s);
                break;
            case 7: {
                int criterio = LerInteiro("Ordenar por 1-codigo  2-nome  3-preco", 1, 3);
                int ordem = ORDEM_CODIGO;
                if (criterio == 2) ordem = ORDEM_NOME;
                if (criterio == 3) ordem = ORDEM_PRECO;
                OrdenarProdutos(s->produtos, ordem);
                SalvarProdutos(s->produtos, s->arqProdutos);
                ImprimirProdutos(s->produtos, NULL);
                break;
            }
        }
    } while (opcao != 0);
}


/* Monta a venda inteira numa Venda separada e so no fim, depois de
   confirmar, mexe no estoque e coloca na lista.  Desistir no meio nao deixa
   sujeira em lugar nenhum. */
static void RegistrarVenda(Sistema *s) {
    printf("\n-- Nova venda --\n");
    if (s->clientes->nrClientes == 0 || s->produtos->nrProdutos == 0) {
        printf("Precisa ter pelo menos um cliente e um produto cadastrados pra vender.\n");
        
        return;
    }

    Cliente *c = EscolherCliente(s->clientes);
    if (!c) return;
    Data data = LerData("Data da venda", 1);

    Venda *v = NovaVenda(s->vendas->proximoCodigo, c->codigo, data);
    if (!v) {
        printf("Erro ao alocar memoria para a venda.\n");
        return;
    }

    printf("\nAgora os produtos. Codigo 0 fecha a venda, -1 mostra a lista.\n");
    
    while (1) {
        int cod = LerInteiro("Produto", -1, MAX_CODIGO);
        if (cod == 0) break;
        if (cod == -1) {
            ImprimirProdutos(s->produtos, NULL);
            continue;
        }

        Produto *p = BuscarProduto(s->produtos, cod);
        if (!p) {
            printf("Nao existe produto com o codigo %d.\n", cod);
            continue;
        }

        // o que ja esta no carrinho desta venda tambem sai do estoque
        int disponivel = p->estoque - QuantidadeNaVenda(v, p->codigo);
        if (disponivel <= 0) {
            printf("%s esta sem estoque, ja era.\n", p->nome);
            continue;
        }

        printf("%s - R$ %.2f (tem %d)\n", p->nome, p->preco, disponivel);
        int qtd = LerInteiro("Quantidade (0 desiste)", 0, disponivel);
        if (qtd == 0) continue;

        if(!AdicionarItem(v, p->codigo, qtd, p->preco)) {
            printf("Erro ao alocar memoria para o item.\n");
            continue;
        }
        printf("Subtotal da venda: R$ %.2f\n", v->total);
    }

    if (v->nrItens == 0) {
        printf("Venda sem nenhum produto, cancelada.\n");
        LiberarVenda(v);
        return;
    }

    ImprimirVenda(v, s->clientes, s->produtos);
    if (!Confirmar("Confirmar a venda?")) {
        printf("Venda cancelada.\n");
        LiberarVenda(v);
        return;
    }

    for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
        Produto *p = BuscarProduto(s->produtos, it->codProduto);
        if (p) p->estoque -= it->quantidade;
    }

    InserirVenda(s->vendas, v);
    SalvarProdutos(s->produtos, s->arqProdutos);
    SalvarVendas(s->vendas, s->arqVendas);
    printf("Venda %d registrada! Dinheiro no caixa.\n", v->codigo);
}


static void MenuVendas(Sistema *s) {
    int opcao;

    do {
        printf("\n--- VENDAS ---\n");
        printf("1. Registrar venda\n");
        printf("2. Listar todas\n");
        printf("3. Ver uma venda\n");
        printf("0. Voltar\n");
        opcao = LerInteiro("Opcao", 0, 3);
        
        if (opcao == 1) {
            RegistrarVenda(s);
        } else if(opcao == 2) {
            
            
            ImprimirVendas(s->vendas, s->clientes, 0, 99999999);
        } else if(opcao == 3) {
            int cod = LerInteiro("Codigo da venda", 1, MAX_CODIGO);
            Venda *v = BuscarVenda(s->vendas, cod);
            if (v) ImprimirVenda(v, s->clientes, s->produtos);
            else printf("Nao existe venda com o codigo %d.\n", cod);
        }
    } while (opcao != 0);
}


static void MenuRelatorios(Sistema *s) {
    int opcao;

    do {
        printf("\n--- RELATORIOS ---\n");
        printf("1. Ranking de clientes\n");
        printf("2. Melhores clientes (maior valor em compras)\n");
        printf("3. Produtos mais vendidos\n");
        printf("4. Vendas por periodo\n");
        printf("5. Faturamento por periodo\n");
        printf("6. Historico de compras de um cliente\n");
        printf("7. Vendas por produto\n");
        printf("8. Produtos com maior faturamento\n");
        printf("0. Voltar\n");
        opcao = LerInteiro("Opcao", 0, 8);

        switch(opcao) {
            case 1: RelatorioRankingClientes(s->clientes, s->vendas); break;
            case 2: RelatorioMelhoresClientes(s->clientes, s->vendas); break;
            case 3: RelatorioProdutosMaisVendidos(s->produtos, s->vendas); break;
            case 4: RelatorioVendasPeriodo(s->clientes, s->vendas); break;
            
            case 5: RelatorioFaturamentoPeriodo(s->vendas); break;
            case 6: RelatorioHistoricoCliente(s->clientes, s->produtos, s->vendas); break;
            case 7: RelatorioVendasPorProduto(s->produtos, s->vendas); break;
            case 8: RelatorioProdutosFaturamento(s->produtos, s->vendas); break;
        }
    } while(opcao != 0);
}


int main(int argc, char *argv[]) {
    Sistema s;
    const char *pasta = (argc > 1) ? argv[1] : ".";

    MontarCaminho(s.arqClientes, TAM_LINHA, pasta, ARQ_CLIENTES);
    MontarCaminho(s.arqProdutos, TAM_LINHA, pasta, ARQ_PRODUTOS);
    MontarCaminho(s.arqVendas, TAM_LINHA, pasta, ARQ_VENDAS);

    s.clientes =  CriarListaClientes();
    s.produtos = CriarListaProdutos();
    s.vendas = CriarListaVendas();
    if (!s.clientes || !s.produtos || !s.vendas) {
        printf("Erro ao alocar memoria, pc da xuxa nao roda.\n");
        LiberarListaClientes(s.clientes);
        LiberarListaProdutos(s.produtos);
        LiberarListaVendas(s.vendas);
        return 1;
    }

    int nc = CarregarClientes(s.clientes, s.arqClientes);
    int np = CarregarProdutos(s.produtos, s.arqProdutos);
    int nv = CarregarVendas(s.vendas, s.arqVendas);
    if (nc < 0 && np < 0 && nv < 0) {
        printf("Nenhum arquivo em '%s', comecando do zero.\n", pasta);
    } else {
        printf("Carregados de '%s': %d cliente(s), %d produto(s), %d venda(s).\n",
               pasta, s.clientes->nrClientes, s.produtos->nrProdutos, s.vendas->nrVendas);
    }

    int opcao;
    do {
        printf("\n=== SISTEMA DE VENDAS 3000 ===\n");
        printf("1. Clientes\n");
        printf("2. Produtos\n");
        printf("3. Vendas\n");
        printf("4. Relatorios\n");
        printf("0. Sair\n");
        opcao = LerInteiro("Opcao", 0, 4);

        switch (opcao) {
            case 1: MenuClientes(&s); break;
            case 2: MenuProdutos(&s); break;
            case 3: MenuVendas(&s); break;
            case 4: MenuRelatorios(&s); break;
        }
    } while (opcao != 0);

    // ja foi tudo gravado a cada alteracao, mas grava de novo por garantia
    SalvarClientes(s.clientes, s.arqClientes);
    SalvarProdutos(s.produtos, s.arqProdutos);
    SalvarVendas(s.vendas, s.arqVendas);

    LiberarListaClientes(s.clientes);
    LiberarListaProdutos(s.produtos);
    LiberarListaVendas(s.vendas);

    printf("Ate a proxima!\n");
    return 0;
}
