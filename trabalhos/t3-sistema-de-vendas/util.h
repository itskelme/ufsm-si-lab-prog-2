#ifndef UTIL_H
#define UTIL_H

#define TAM_LINHA 256
#define TAM_NOME 60
#define MAX_CODIGO 999999

#define ORDEM_CODIGO 1
#define ORDEM_NOME   2
#define ORDEM_PRECO  3

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;


/* Leitura do teclado: tudo e' lido linha a linha com fgets e convertido
   depois, assim uma letra no lugar de um numero nao trava o programa. */
void LerLinha(char *destino, int tamanho);
int LerTexto(const char *rotulo, char *destino, int tamanho, int obrigatorio);
int LerInteiro(const char *rotulo, int minimo, int  maximo);
double LerReal(const char *rotulo, double minimo, double maximo);
Data LerData(const char *rotulo, int aceitaHoje);
int Confirmar(const char *pergunta);

int DataValida(Data d);
int DataParaInt(Data d);
int TextoParaData(const char *texto, Data *d);
Data Hoje(void);

void CopiaTexto(char *destino, const char *origem, int tamanho);
int ComparaSemCaixa(const char *a, const char *b);
int ContemTexto(const char *texto, const char *trecho);
int PegarCampo(char **cursor, char *destino, int tamanho);
void MontarCaminho(char *destino, int tamanho, const char *pasta, const char *arquivo);
double Arredonda(double valor);

#endif
