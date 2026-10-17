#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_CHAR 1024
#define MAX_HEIGHT 5
//QUANTIDADE = SUBLINHAS
 
//Struct que representa 
typedef struct
{
    char* name;
    char** refs;
    int nvalores;
    int maximo;
}tabela;

typedef struct no
{
    int atributo;
    int classe;
    int nfilhos;
    struct no** filhos;
} no;

no* nova_folha(int classe)
{
    no* folha = malloc(sizeof(no));

    folha->atributo = -1;
    folha->classe = classe;
    folha->nfilhos = 0;
    folha->filhos = NULL;

    return folha;
}

no* novo_no(int classe, int nfilhos, int atributo)
{
    no* interno = malloc(sizeof(no));

    interno->atributo = atributo;
    interno->classe = classe;
    interno->nfilhos = nfilhos;
    interno->filhos = calloc(nfilhos, sizeof(no*));

    return interno;
}

//traduçao da tabela de afirmaçoes em inteiros simples de computar as equaçoes de ganho de informação
int tradutor(tabela** trad, char* token, int coluna)
{
    for(int i = 0; i < trad[coluna]->nvalores; i++)
    {
        if(strcmp(trad[coluna]->refs[i], token) == 0)
        {
            return i;
        }
    }
    
    return -1;
}

double entropia(int* cont, int total, int num_classes)
{
    double entropia = 0.0;
    
    for(int classe = 0; classe < num_classes; classe++)
    {
        if(cont[classe] > 0)
        {
            double atual = (double) cont[classe] / (double) total;
            
            entropia -= atual * log2(atual);
        }
    }
    
    return entropia;
}

//A funçao particionar determina os vetores int* linhas para cada um dos ramos criados ao dividir uma árvore de decisão.
int** particionar(int** matriz, int* linhas, int sublinhas, int coluna, int natributos, int* tamanhos)
{
    int** ramos = malloc(natributos * sizeof(int*));
    for(int atr_atual = 0; atr_atual < natributos; atr_atual++)
    {
        ramos[atr_atual] = calloc(sublinhas, sizeof(int));
        for(int posicao = 0; posicao < sublinhas; posicao++)
        {
            if(matriz[linhas[posicao]][coluna] == atr_atual)
            {
                ramos[atr_atual][tamanhos[atr_atual]] = linhas[posicao];
                tamanhos[atr_atual]++; 
            }
        }
    }

    return ramos;
}

void contar_classes(int** matriz, int* linhas, int total, int classes, int* cont)
{
    memset(cont, 0, classes * sizeof(int));
    for(int i = 0; i < total; i++)
    {
        cont[matriz[linhas[i]][0]]++;   
    }
}

double ganho_informacao(int** matriz, tabela** trad, int* linhas, int coluna,  int total)
{
    int num_classes = trad[0]->nvalores;
    int num_atributos = trad[coluna]->nvalores;
    
    int* total_classes = calloc(num_classes, sizeof(int));
    int* contagem = calloc(num_atributos * num_classes, sizeof(int));
    
    for(int i = 0; i < total; i++)
    {
        int classe = matriz[linhas[i]][0];
        int valor = matriz[linhas[i]][coluna];
        
        total_classes[classe]++;
        contagem[valor * num_classes + classe]++;
    }
    
    //Fórmula do Ganho de Informação
    //IG = H(S) - somatório( |S_v|/|S| * H(S_v) ) 
    
    //H(S) da formula:
    double resultado = entropia(total_classes, total, num_classes);
    
    for(int atributo = 0; atributo < num_atributos; atributo++)
    {
        int total_valor = 0;
        
        for(int classe = 0; classe < num_classes; classe++)
        {
            total_valor += contagem[atributo * num_classes + classe];
        }
        
        if(total_valor > 0)
        {
            double peso = (double) total_valor / total;
            
            resultado -= peso * entropia(contagem + (atributo * num_classes), total_valor, num_classes);
        }
    }
    
    free(total_classes);
    free(contagem);
    return resultado;
}

int melhor_atributo(int** matriz, tabela** trad, int* linhas, int* atributos_usado, int sublinhas, int ncolunas, double* melhor_ig)
{
    int melhor_indice = -1;
    double melhor_ganho = -1.0;

    for(int coluna = 1; coluna < ncolunas; coluna++)
    {
        if(!atributos_usado[coluna])
        {
            double temp = ganho_informacao(matriz, trad, linhas, coluna, sublinhas);
            printf("%.3lf\n", temp);

            if(temp > melhor_ganho)
            {
                melhor_ganho = temp;
                melhor_indice = coluna;
            }
        } 
    }

    *melhor_ig = melhor_ganho;
    printf("%.3lf %d\n", *melhor_ig, melhor_indice);
    return melhor_indice;
}



void setup(tabela** t, char* token, int ind)
{  
    if(t[ind] == NULL)
    {
        t[ind] = malloc(sizeof(tabela));
        t[ind]->name = strdup(token);
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
        t[ind]->refs[t[ind]->nvalores] = strdup(token);
        t[ind]->nvalores++;
    }
}

int main(int argc, char* argv[])
{

    //Checa se o programa não recebeu um arquivo .CSV
    if(argc < 2)
    {
        fprintf(stderr, "Formato do programa: %s arquivo.csv\n", argv[0]);
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

    int** matriz = malloc(nlinhas * sizeof(int*));
    if(matriz == NULL)
    {
        perror("malloc");
        exit(1);
    }

    for(int i = 0; i < nlinhas; i++)
    {
        matriz[i] = malloc(ncolunas * sizeof(int));
        if(matriz[i] == NULL)
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
        int coluna = 0;
        for(token = strtok(line, ","); token != NULL && coluna < ncolunas; token = strtok(NULL, ","))
        {
            matriz[linha][coluna] = tradutor(trad, token, coluna);
            coluna++;
        }
    }

    int* linhas = malloc(nlinhas * sizeof(int));
    
    for(int i = 0; i < nlinhas; i++)
    {
        linhas[i] = i;
    }

    int* atributo_usado = calloc(ncolunas, sizeof(int));

    double melhor_ig;
    int melhor_indice = melhor_atributo(matriz, trad, linhas, atributo_usado, nlinhas, ncolunas, &melhor_ig);

    int* tamanhos = calloc(trad[melhor_indice]->nvalores, sizeof(int));

    int* linhas_base = malloc(nlinhas * sizeof(int));
    for(int i = 0; i < nlinhas; i++)
    {
        linhas_base[i] = i;
    }
    int** linhas_ramos = particionar(matriz, linhas_base, nlinhas, melhor_indice, trad[melhor_indice]->nvalores, tamanhos);

    for(int i = 0; i < trad[melhor_indice]->nvalores; i++)
    {
        for(int j = 0; j < tamanhos[i]; j++)
        {
            printf("%d ", linhas_ramos[i][j]);
        }
        puts("");
    }

    fclose(f);

    return 0;
}