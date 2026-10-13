#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

typedef struct
{
    char* name;
    char** refs;
    int nvalores;
    int maximo;
}tabela;

void setup(tabela** coluna, char* token, int ind)
{  
    if(coluna[ind] == NULL)
    {
        coluna[ind] = malloc(sizeof(tabela));
        coluna[ind]->name = token;
        coluna[ind]->refs = realloc(NULL, n);
        coluna[ind]->nvalores = 0;
        coluna[ind]->maximo = 0;
    }
    else
    {
        if(coluna[ind]->nvalores == coluna[ind]->maximo)
        {
            coluna[ind]->maximo = coluna[ind]->maximo ? coluna[ind]->maximo * 2 : 4;
            coluna[ind]->refs = realloc(coluna[ind]->refs, coluna[ind]->maximo);
        }

        int i = 0;
        while(coluna[ind]->refs[i] != NULL)
        {
            if(strncmp(token, coluna[ind]->refs[i], sizeof(token)) != 0)
            {
                return;
            }
            i++;
        }
        coluna[ind]->refs[i] = token;
        coluna[ind]->nvalores++;
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

    tabela** coluna = malloc(sizeof(tabela*));

    char line[MAX];
    fgets(line, sizeof(line), f);
    line[strcspn(line, "\r\n")] = '\0';

    char* tok;
    tok = strtok(line, ",");

    while(tok != NULL)
    {
        setup(coluna, tok, ind);
        tok = strtok(NULL, ",");
        i++;
    }

    while(fgets(line, sizeof(line), f) != NULL)
    {
        int j = 0;
        for(tok = strtok(line, ","), tok != NULL, tok = strtok(NULL, ","))
        {
            new_variable(coluna, tok, j);
            j++;
        }
    }


    fclose(f);

    return 0;
}