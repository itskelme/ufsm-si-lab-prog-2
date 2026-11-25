#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "util.h"


void LerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) == NULL) {
        /* acabou a entrada (Ctrl+D, Ctrl+Z ou arquivo redirecionado). Como os
           dados sao gravados a cada alteracao, da pra sair direto daqui */
        printf("\nEntrada encerrada.\n");
        exit(0);
    }

    char *fim = strchr(destino, '\n');
    if (fim != NULL) {
        *fim = '\0';
    } else {
        // linha maior que o buffer: joga fora o resto
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    int n = (int)strlen(destino);
    if (n > 0 && destino[n - 1] == '\r') destino[n - 1] = '\0';
}


/* Le um texto e tira os espacos das pontas. O ';' vira ',' porque ele e' o
   separador dos campos nos arquivos. Devolve quantos caracteres ficaram. */
int LerTexto(const char *rotulo, char *destino, int tamanho, int obrigatorio) {
    char linha[TAM_LINHA];

    while (1) {
        printf("%s: ", rotulo);
        LerLinha(linha, sizeof(linha));

        char *ini = linha;
        while (*ini == ' ' || *ini == '\t') ini++;
        int n = (int)strlen(ini);
        while (n > 0 && (ini[n - 1] == ' ' || ini[n - 1] == '\t')) n--;
        ini[n] = '\0';

        if (n == 0 && obrigatorio) {
            printf("Esse campo nao pode ficar vazio.\n");
            continue;
        }
        if (n >= tamanho) {
            printf("Texto muito grande, o maximo sao %d caracteres.\n", tamanho - 1);
            continue;
        }

        for (int i = 0; ini[i] != '\0'; i++) {
            if (ini[i] == ';') ini[i] = ',';
        }
        strcpy(destino, ini);
        return n;
    }
}


int LerInteiro(const char *rotulo, int minimo, int maximo) {
    char linha[TAM_LINHA];
    char sobra;
    int valor;

    while (1) {
        printf("%s: ", rotulo);
        LerLinha(linha, sizeof(linha));
        if (sscanf(linha, "%d %c", &valor, &sobra) == 1 && valor >= minimo && valor <= maximo) {
            return valor;
        }
        printf("Valor invalido, digite um numero de %d a %d.\n", minimo, maximo);
    }
}


double LerReal(const char *rotulo, double minimo, double maximo) {
    char linha[TAM_LINHA];
    char sobra;
    double valor;

    while (1) {
        printf("%s: ", rotulo);
        LerLinha(linha, sizeof(linha));

        // aceita 12,50 tambem, que e' como todo mundo digita preco
        for (int i = 0; linha[i] != '\0'; i++) {
            if (linha[i] == ',') linha[i] = '.';
        }
        if (sscanf(linha, "%lf %c", &valor, &sobra) == 1 && valor >= minimo && valor <= maximo) {
            return valor;
        }
        printf("Valor invalido, digite um valor de %.2f a %.2f.\n", minimo, maximo);
    }
} 


static int DiasNoMes(int mes, int ano) {
    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2 && ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0)) return 29;
    return dias[mes - 1];
}


int DataValida(Data d) {
    if(d.ano < 1900 || d.ano > 2100) return 0;
    if (d.mes < 1 || d.mes > 12) return 0;
    return (d.dia >= 1 && d.dia <= DiasNoMes(d.mes, d.ano));
}


/* 25/11/2026 vira 20261125. Assim da pra comparar duas datas com < e > */
int DataParaInt(Data d) {
    return d.ano * 10000 + d.mes * 100 + d.dia;
}


int TextoParaData(const char *texto, Data *d) {
    char sobra;
    if (sscanf(texto, "%d/%d/%d %c", &d->dia, &d->mes, &d->ano, &sobra) != 3) return 0;
    return DataValida(*d);
}


Data Hoje(void) {
    time_t agora = time(NULL);
    struct tm *tm = localtime(&agora);
    Data d;

    d.dia = tm->tm_mday;
    d.mes = tm->tm_mon + 1;
    d.ano = tm->tm_year + 1900;
    return d;
}


Data LerData(const char *rotulo, int aceitaHoje) {
    char linha[TAM_LINHA];
    Data d;
    
    while (1) {
        if (aceitaHoje) printf("%s (dd/mm/aaaa, Enter = hoje): ", rotulo);
        else printf("%s (dd/mm/aaaa): ", rotulo);
        LerLinha(linha, sizeof(linha));

        if (aceitaHoje && linha[0] == '\0') return Hoje();
        if (TextoParaData(linha, &d)) return d;
        printf("Data invalida.\n");
    }
}


int Confirmar(const char *pergunta) {
    char linha[TAM_LINHA];

    printf("%s (s/n): ", pergunta);
    LerLinha(linha, sizeof(linha));
    return (linha[0] == 's' || linha[0] == 'S');
}


/* strncpy nao coloca o '\0' quando o texto enche o vetor, entao a ultima
   posicao e' fechada na mao */
void CopiaTexto(char *destino, const char *origem, int tamanho) {
    strncpy(destino, origem, (size_t)tamanho - 1);
    destino[tamanho - 1] = '\0';
}


int ComparaSemCaixa(const char *a, const char *b) {
    while(*a != '\0' && tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
        a++;
        b++;
    }
    return tolower((unsigned char)*a)  - tolower((unsigned char)*b);
}


/* "maria" acha "Ana Maria Souza" */
int ContemTexto(const char *texto, const char *trecho) {
    int n = (int)strlen(trecho);
    if (n == 0) return 1;

    for (int i = 0; texto[i] != '\0'; i++) {
        int j = 0;
        while (j < n && texto[i + j] != '\0' &&
               tolower((unsigned char)texto[i + j]) == tolower((unsigned char)trecho[j])) {
            j++;
        }
        if (j == n) return 1;
    
    
    }
    return 0;
}


/* Copia o proximo campo de uma linha "a;b;c" pra destino e avanca o cursor.
   Diferente do strtok aceita campo vazio (cliente sem email, por exemplo).
   Devolve 0 quando nao tem mais campo nenhum. */
int PegarCampo(char **cursor, char *destino, int tamanho) {
    char *p = *cursor;
    int n = 0;

    if (p == NULL) return 0;

    while (*p != '\0' && *p != ';') {
        if (n < tamanho - 1) destino[n++] = *p;
        p++;
    }
    destino[n] = '\0';

    if (*p == ';') *cursor = p + 1;
    else *cursor = NULL;
    return 1;
}


void MontarCaminho(char *destino, int tamanho, const char *pasta, const char *arquivo) {
    snprintf(destino, (size_t)tamanho, "%s/%s", pasta, arquivo);
}



// arredonda para centavos sem precisar da math.h
double Arredonda(double valor) {
    return (double)(long long)(valor * 100.0 + 0.5) / 100.0;
}
