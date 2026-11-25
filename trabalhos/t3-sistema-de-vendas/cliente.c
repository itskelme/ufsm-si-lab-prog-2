#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cliente.h"


ListaClientes* CriarListaClientes(void) {
    ListaClientes *l = (ListaClientes*)malloc(sizeof(ListaClientes));
    if (!l) return NULL;

    l->inicio = NULL;
    l->nrClientes = 0;
    l->proximoCodigo = 1;
    return l;
}


void LiberarListaClientes(ListaClientes *l) {
    if (!l) return;

    Cliente *atual = l->inicio;
    while(atual != NULL) {
        Cliente *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(l);
}


Cliente* NovoCliente(int codigo, const char *nome, const char *cpf, const char *telefone, const char *email) {
    Cliente *c = (Cliente*)malloc(sizeof(Cliente));
    if (!c) return NULL;

    c->codigo = codigo;
    CopiaTexto(c->nome, nome, TAM_NOME);
    CopiaTexto(c->cpf, cpf, TAM_CPF);
    CopiaTexto(c->telefone, telefone, TAM_TELEFONE);
    CopiaTexto(c->email, email, TAM_EMAIL);
    c->prox = NULL;
    return c;
}


/* Insere no fim. Como o codigo so cresce, a lista fica em ordem de codigo
   ate alguem mandar ordenar por nome. */
void InserirCliente(ListaClientes *l, Cliente *c) {
    c->prox = NULL;

    if (l->inicio == NULL) {
        l->inicio = c;
    } else {
        Cliente *p = l->inicio;
        while (p->prox != NULL) p = p->prox;
        p->prox = c;
    }

    l->nrClientes++;
    if (c->codigo >= l->proximoCodigo) l->proximoCodigo = c->codigo + 1;
}


Cliente* BuscarCliente(ListaClientes *l, int codigo) {
    for (Cliente *p = l->inicio; p != NULL; p = p->prox) {
        if (p->codigo == codigo) return p;
    }
    return NULL;
}


Cliente* BuscarClientePorCpf(ListaClientes *l, const char *cpf) {
    for (Cliente *p = l->inicio; p != NULL; p = p->prox) {
        if (strcmp(p->cpf, cpf) == 0) return p;
    }
    return NULL;
}


/* O codigo removido nao volta pro proximoCodigo de proposito: assim um
   cliente novo nunca herda o codigo de outro. */
int RemoverCliente(ListaClientes *l, int codigo) {
    Cliente *ant = NULL;
    Cliente *p = l->inicio;

    while (p != NULL && p->codigo != codigo) {
        ant = p;
        p = p->prox;
    }
    if (p == NULL) return 0;

    if (ant == NULL) l->inicio = p->prox;
    else ant->prox = p->prox;

    free(p);
    l->nrClientes--;
    return 1;
}


static int ClienteVemAntes(Cliente *a, Cliente *b, int criterio) {
    if (criterio == ORDEM_NOME) {
        int cmp = ComparaSemCaixa(a->nome, b->nome);
        if (cmp != 0) return cmp < 0;
    }
    return a->codigo < b->codigo;
}


/* Insertion sort mexendo so nos ponteiros: tira um no de cada vez da lista
   velha e encaixa no lugar certo da lista nova. Nome igual fica na ordem
   em que ja estava. */
void OrdenarClientes(ListaClientes *l, int criterio) {
    Cliente *ordenada = NULL;
    Cliente *atual = l->inicio;

    while (atual != NULL) {
        Cliente *prox = atual->prox;

        if (ordenada == NULL || ClienteVemAntes(atual, ordenada, criterio)) {
            atual->prox = ordenada;
            ordenada = atual;
        } else {
            Cliente *p = ordenada;
            while (p->prox != NULL && !ClienteVemAntes(atual, p->prox, criterio)) {
                p = p->prox;
            }
            atual->prox = p->prox;
            p->prox = atual;
        }
        atual = prox;
    }

    l->inicio = ordenada;
}


/* Copia so os numeros da entrada ("529.982.247-25" vira "52998224725").
   Devolve quantos digitos tinha, mesmo que nao caibam todos. */
int SoDigitos(const char *entrada, char *saida, int tamanho) {
    int n = 0;
    for (int i = 0; entrada[i] != '\0'; i++) {
        if (isdigit((unsigned char)entrada[i])) {
            if (n < tamanho - 1) saida[n] = entrada[i];
            n++;
        }
    }
    saida[(n < tamanho - 1) ? n : tamanho - 1] = '\0';
    return n;
}



/* Confere os dois digitos verificadores do CPF.
   Cada digito e' a soma dos anteriores com pesos decrescentes, vezes 10,
   resto da divisao por 11 (e se der 10 vira 0). */
int CpfValido(const char *cpf) {
    if (strlen(cpf) != 11) return 0;

    // 111.111.111-11 passa na conta mas nao e' CPF de ninguem
    int iguais = 1;
    for (int i = 1; i < 11; i++) {
        if (cpf[i] != cpf[0]) iguais = 0;
    }
    if (iguais) return 0;

    for (int d = 9; d <= 10; d++) {
        int soma = 0;
        for (int i = 0; i < d; i++) {
            soma += (cpf[i] - '0') * (d + 1 - i);
        
        
        }
        int dv = (soma * 10) % 11;
        if (dv == 10) dv = 0;
        if (dv != cpf[d] - '0') return 0;
    }
    return 1;
}


// saida precisa de pelo menos 15 posicoes
void FormataCpf(const char *cpf, char *saida) {
    if(strlen(cpf) != 11) {
        strcpy(saida, cpf);
        return;
    }
    sprintf(saida, "%.3s.%.3s.%.3s-%.2s", cpf, cpf + 3, cpf + 6, cpf + 9);
}


void ImprimirCliente(Cliente *c) {
    char cpf[16];
    FormataCpf(c->cpf, cpf);

    printf("\nCodigo:   %d\n", c->codigo);
    printf("Nome:     %s\n", c->nome);
    printf("CPF:      %s\n", cpf);
    printf("Telefone: %s\n", (c->telefone[0] != '\0') ? c->telefone : "-");
    printf("Email:    %s\n", (c->email[0] != '\0') ? c->email : "-");
}


static void LinhaClientes(void) {
    printf("+------+--------------------------------+----------------+-----------------+\n");
}


void ImprimirClientes(ListaClientes *l, const char *filtroNome) {
    int mostrados = 0;

    if (l->nrClientes == 0) {
        printf("\nNenhum cliente cadastrado ainda.\n");
        return;
    }

    printf("\n");
    LinhaClientes();
    printf("| %-4s | %-30s | %-14s | %-15s |\n", "Cod", "Nome", "CPF", "Telefone");
    LinhaClientes(); 

    for (Cliente *c = l->inicio; c != NULL; c = c->prox) {
        if (filtroNome != NULL && !ContemTexto(c->nome, filtroNome)) continue;

        char cpf[16];
        FormataCpf(c->cpf, cpf);
        printf("| %4d | %-30.30s | %-14s | %-15.15s |\n", c->codigo, c->nome, cpf, c->telefone);
        mostrados++;
    }

    LinhaClientes();
    printf("%d cliente(s)\n", mostrados);
}


/* Pergunta o codigo ate achar um cliente que existe. 0 desiste e -1 mostra
   a lista, pra ninguem precisar decorar codigo. */
Cliente* EscolherCliente(ListaClientes *l) {
    if(l->nrClientes == 0) {
        printf("\nNenhum cliente cadastrado ainda.\n");
        return NULL;
    }

    while (1) {
        int cod = LerInteiro("Codigo do cliente (0 cancela, -1 lista)", -1, MAX_CODIGO);
        if (cod == 0) return NULL;
        if (cod == -1) {
            ImprimirClientes(l, NULL);
            continue;
        }

        Cliente *c = BuscarCliente(l, cod);
        if (c != NULL) return c;
        printf("Nao existe cliente com o codigo %d.\n", cod);
    }
}
