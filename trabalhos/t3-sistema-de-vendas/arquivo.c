#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

/* Formato dos arquivos: texto, um registro por linha, campos separados por ';'

     clientes.txt   codigo;nome;cpf;telefone;email
     produtos.txt   codigo;nome;preco;estoque
     vendas.txt     V;codigo;codCliente;dd/mm/aaaa
                    I;codProduto;quantidade;precoUnitario   (uma linha por item, logo abaixo da venda)

   Linha estragada e' ignorada com um aviso e o resto do arquivo continua valendo.
   Da pra abrir e corrigir os arquivos no bloco de notas se precisar.  */


static void TiraFimDeLinha(char *linha) {
    int n = (int)strlen(linha);
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[--n] = '\0';
    }
}

static int TextoParaInt(const char *texto, int *valor) {
    char sobra;
    return sscanf(texto, "%d %c", valor, &sobra) == 1;
}


static int TextoParaReal(const char *texto, double *valor) {
    char sobra;
    return sscanf(texto, "%lf %c", valor, &sobra) == 1;
}


static void AvisoLinha(const char *caminho, int nrLinha) {
    printf("Aviso: %s, linha %d estragada, foi ignorada\n", caminho, nrLinha);
}


int CarregarClientes(ListaClientes *l, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return -1;

    char linha[TAM_LINHA * 2];
    char campo[TAM_LINHA];
    int nrLinha = 0;
    int lidos = 0;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        nrLinha++;
        TiraFimDeLinha(linha);
        if (linha[0] == '\0') continue;

        char *cursor = linha;
        char nome[TAM_NOME], cpf[TAM_CPF], telefone[TAM_TELEFONE], email[TAM_EMAIL];
        int codigo = 0;

        int ok = PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &codigo) && codigo > 0
              && PegarCampo(&cursor, nome, sizeof(nome)) && nome[0] != '\0'
              && PegarCampo(&cursor, cpf, sizeof(cpf))
              && PegarCampo(&cursor, telefone, sizeof(telefone))
              && PegarCampo(&cursor, email, sizeof(email));

        // codigo repetido tambem conta como estragado, senao a busca acharia so o primeiro
        if (!ok || BuscarCliente(l, codigo) != NULL) {
            AvisoLinha(caminho, nrLinha);
            continue;
        }

        Cliente *c = NovoCliente(codigo, nome, cpf, telefone, email);
        if (!c) break;
        InserirCliente(l, c);
        lidos++;
    }
    
    fclose(f);
    return lidos;
}


int CarregarProdutos(ListaProdutos *l, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return -1;

    char linha[TAM_LINHA * 2];
    char campo[TAM_LINHA];
    int nrLinha = 0;
    int lidos = 0;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        nrLinha++;
        TiraFimDeLinha(linha);
        if (linha[0] == '\0') continue;

        char *cursor = linha;
        char nome[TAM_NOME];
        int codigo = 0, estoque = 0;
        double preco = 0.0;

        int ok = PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &codigo) && codigo > 0
              && PegarCampo(&cursor, nome, sizeof(nome)) && nome[0] != '\0'
              && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaReal(campo, &preco) && preco >= 0
              && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &estoque) && estoque >= 0;

        if (!ok || BuscarProduto(l, codigo) != NULL) {
            AvisoLinha(caminho, nrLinha);
            continue;
        }

        Produto *p = NovoProduto(codigo, nome, Arredonda(preco), estoque);
        if (!p) break;
        InserirProduto(l, p);
        lidos++;
    
    }

    fclose(f);
    return lidos;
}



/* Coloca na lista a venda que acabou de ser lida. Venda sem item nenhum ou
   com codigo repetido e' descartada. */
static int FechaVendaLida(ListaVendas *l, Venda *v, const char *caminho) {
    if (v->nrItens == 0 || BuscarVenda(l, v->codigo) != NULL) {
        printf("Aviso: %s, venda %d sem itens ou repetida, foi ignorada\n", caminho, v->codigo);
        LiberarVenda(v);
        return 0;
    }
    InserirVenda(l, v);
    return 1;
}


int CarregarVendas(ListaVendas *l, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return -1;

    char linha[TAM_LINHA * 2];
    char campo[TAM_LINHA];
    int nrLinha = 0;
    int lidas = 0;
    Venda *atual = NULL;    /* venda que esta recebendo os itens */
    int pulando = 0;        /* 1 = o cabecalho da venda estava estragado, pula os itens dela */

    while (fgets(linha, sizeof(linha), f) != NULL) {
        nrLinha++;
        TiraFimDeLinha(linha);
        if (linha[0] == '\0') continue;

        char *cursor = linha;
        PegarCampo(&cursor, campo, sizeof(campo));

        if (strcmp(campo, "V") == 0) {
            if (atual != NULL) lidas += FechaVendaLida(l, atual, caminho);
            atual = NULL;

            int codigo = 0, codCliente = 0;
            Data data = {0, 0, 0};
            int ok = PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &codigo) && codigo > 0
                  && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &codCliente) && codCliente > 0
                  && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaData(campo, &data);

            if (!ok) {
                AvisoLinha(caminho, nrLinha);
                pulando = 1;
                continue;
            }

            atual = NovaVenda(codigo, codCliente, data);
            if (!atual) break;
            pulando = 0;

        } else if (strcmp(campo, "I") == 0) {
            if (pulando) continue;

            int codProduto = 0, quantidade = 0;
            double preco = 0.0;
            int ok = atual != NULL
                  && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &codProduto) && codProduto > 0
                  && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaInt(campo, &quantidade) && quantidade > 0
                  && PegarCampo(&cursor, campo, sizeof(campo)) && TextoParaReal(campo, &preco) && preco >= 0;

            if (!ok) {
                AvisoLinha(caminho, nrLinha);
                continue;
            }
            AdicionarItem(atual, codProduto, quantidade, Arredonda(preco));

        } else {
            AvisoLinha(caminho, nrLinha);
        }
    }
    
    if (atual != NULL) lidas += FechaVendaLida(l, atual, caminho);

    fclose(f);
    return lidas;
}


int SalvarClientes(ListaClientes *l, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        printf("Erro: nao consegui gravar %s\n", caminho);
        return 0;
    }

    for (Cliente *c = l->inicio; c != NULL; c = c->prox) {
        fprintf(f, "%d;%s;%s;%s;%s\n", c->codigo, c->nome, c->cpf, c->telefone, c->email);
    }

    fclose(f);
    return 1;
}


int SalvarProdutos(ListaProdutos *l, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        printf("Erro: nao consegui gravar %s\n", caminho);
        return 0;
    }

    for (Produto *p = l->inicio; p != NULL; p = p->prox) {
        fprintf(f, "%d;%s;%.2f;%d\n", p->codigo, p->nome, p->preco, p->estoque);
    }

    fclose(f);
    return 1;
}


int SalvarVendas(ListaVendas *l, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        printf("Erro: nao consegui gravar %s\n", caminho);
        return 0;
    }

    for (Venda *v = l->inicio; v != NULL; v = v->prox) {
        fprintf(f, "V;%d;%d;%02d/%02d/%04d\n", v->codigo, v->codCliente, v->data.dia, v->data.mes, v->data.ano);
        for (ItemVenda *it = v->itens; it != NULL; it = it->prox) {
            fprintf(f, "I;%d;%d;%.2f\n", it->codProduto, it->quantidade, it->precoUnitario);
        }
    }

    fclose(f);
    return 1;
}
