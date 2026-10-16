#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_CHAR 1024
#define MAX_HEIGHT 5
 
typedef struct
{
    char* name;
    char** refs;
    int nvalores;
    int maximo;
}tabela;

typedef struct
{
    int* col_usados;

}

typedef struct node
{


    node** filhos;
}

//traduçao da tabela de afirmaçoes em inteiros simples de computar as equaçoes de ganho de informação
int tradutor(tabela** trad, char* token, int coluna)
{
    for(int i = 0; i < trad[coluna]->nvalores; i++)
    {
        if(strcmp(trad[coluna]->refs[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

double entropia(int* cont, int total, int classes)
{
    double entropia = 0.0;

    for(int i = 0; i < classes; i++)
    {
        if(cont[classe] > 0)
        {
            double atual = (double) cont[classe] / (double) total;
    
            entropia -= atual * log2(atual);
        }
    }

    return h;
}

void contar_classes(int** matriz, int* linhas, int total, int classes, int* cont)
{
    memset(cont, 0, classes * sizeof(int));
    for(int i = 0; i < total; i++)
    {
        cont[matriz[linha[i]][0]]++;   
    }
}



void setup(tabela** t, char* token, int ind)
{  
    if(t[ind] == NULL)
    {
        t[ind] = malloc(sizeof(tabela));
        t[ind]->name = strdup(token)
        t[ind]->refs = NULL;
        t[ind]->nvalores = 0;
        t[ind]->maximo = 0;
    }
    else
    {
        if(t[ind]->nvalores == t[ind]->maximo)
        {
            int antigo = t[ind]->maximo;
            int novo = antigo ? antigo * 2 : 4;

            char** tmp = realloc(t[ind]->refs, novo * sizeof(char*));
            if(tmp == NULL)
            {
                perror("realloc");
                exit(1);
            }
            t[ind]->refs = tmp;
            t[ind]->maximo = novo;

            memset(t[ind]->refs + antigo, 0, (novo - antigo) * sizeof(char*));
        }

        for(int i = 0; i < t[ind]->nvalores; i++)
        {                           
            if(strcmp(token, t[ind]->refs[i]) == 0)
            {
                return;
            }
        }
        t[ind]->refs[t[ind]->nvalores] = strdup(token)
        t[ind]->nvalores++;
    }
}

int main(int argc, char* argv[])
{
    //Checa se o programa não recebeu um arquivo .CSV
    if(argc < 2)
    {
        fprintf(stderr, "Formato do programa: %s arquivo.csv", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if(f == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    tabela** trad = NULL;
    int tamanho = 0; 
    int ncolunas = 0;

    char line[MAX_CHAR];
    fgets(line, sizeof(line), f);
    line[strcspn(line, "\r\n")] = '\0';

    char* token;
    token = strtok(line, ",");

    while(token != NULL)
    {
        if(ncolunas == tamanho)
        {
            int antigo = tamanho;
            tamanho = tamanho ? tamanho + 2 : 4;

            tabela** novo = realloc(trad, tamanho * sizeof(tabela*));
            if(novo == NULL)
            {
                perror("realloc");
                return 1;
            }

            memset(novo + antigo, 0, (tamanho - antigo) * sizeof(tabela*));
            trad = novo;
            
        }  
        setup(trad, token, ncolunas);
        token = strtok(NULL, ",");
        ncolunas++;
    }

    int nlinhas = 0;
    int j = 0;

    while(fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';
        for(token = strtok(line, ","); token != NULL; token = strtok(NULL, ","))
        {
            setup(trad, token, j);
            j++;
        }
        
        j = 0;
        nlinhas++;
    }

    rewind(f);

    int** m = malloc(nlinhas * sizeof(int*));
    if(m == NULL)
    {
        perror("malloc");
        exit(1);
    }

    for(int i = 0; i < nlinhas; i++)
    {
        m[i] = malloc(ncolunas * sizeof(int));
        if(m[i] == NULL)
        {
            perror("malloc");
            exit(1);
        }
    }

    fgets(line, sizeof(line), f);

    for(int linha = 0; linha < nlinhas; linha++)
    {
        fgets(line,sizeof(line), f);
        line[strcspn(line, "\r\n")] = '\0';
        token = strtok(line, ",");
        int coluna = 0;
        for(token = strtok(line, ","); token != NULL && coluna < ncolunas; token = strtok(NULL, ","))
        {
            m[linha][coluna] = tradutor(trad, token, coluna);
            coluna++;
        }
    }

    fclose(f);

    return 0;
}