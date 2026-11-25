#ifndef CLIENTE_H
#define CLIENTE_H

#include "util.h"

#define TAM_CPF 12        /* 11 digitos + '\0' */
#define TAM_TELEFONE 20
#define TAM_EMAIL 60

typedef struct Cliente {
    int codigo;
    char nome[TAM_NOME];
    char cpf[TAM_CPF];    /* so os digitos, a mascara  e' colocada na hora de imprimir */
    char telefone[TAM_TELEFONE];
    char email[TAM_EMAIL];
    struct Cliente *prox;
} Cliente;

typedef struct {
    Cliente *inicio;
    int nrClientes;
    int proximoCodigo; 
} ListaClientes;


ListaClientes* CriarListaClientes(void);
void LiberarListaClientes(ListaClientes *l);

Cliente* NovoCliente(int codigo, const char *nome, const char *cpf, const char *telefone, const char *email);
void InserirCliente(ListaClientes *l, Cliente *c);
Cliente* BuscarCliente(ListaClientes *l, int codigo);
Cliente* BuscarClientePorCpf(ListaClientes *l, const char *cpf);
int RemoverCliente(ListaClientes *l, int codigo);
void OrdenarClientes(ListaClientes *l, int criterio);

int CpfValido(const char *cpf);
int SoDigitos(const char *entrada, char *saida, int tamanho);
void FormataCpf(const char *cpf, char *saida);

void ImprimirCliente(Cliente *c);
void ImprimirClientes(ListaClientes *l, const char *filtroNome);
Cliente* EscolherCliente(ListaClientes *l);

#endif
