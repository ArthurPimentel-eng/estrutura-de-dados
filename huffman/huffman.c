
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct no{
    unsigned char caracter;
    unsigned long frequencia;
    struct no *esq, *dir, *prox;
}no;

typedef struct Lista{
    no *inicio;
    unsigned long long tamanho;
}Lista;

void iniciarLista(Lista* lista){
    lista->inicio = NULL;
    lista->tamanho = 0;
}

void inserirListaOrdenadamente(Lista* lista, no *novoNo){
    no *aux;

    //primeiro elemento da lista
    if(lista->inicio == NULL){
        lista->inicio = novoNo;

    //inserir no inicio da lista
    } else if(novoNo->frequencia < lista->inicio->frequencia){
        novoNo->prox = lista->inicio;
        lista->inicio = novoNo;

    //inserir no meio ou final da lista
    } else{
        aux = lista->inicio;
        //procurar a posicao de insercao do novo no
        while(aux->prox != NULL && aux->prox->frequencia <= novoNo->frequencia){
            aux = aux->prox;
        }
        novoNo->prox = aux->prox;
        aux->prox = novoNo;

    }
    lista->tamanho++;
    return;
}

//criar nó e chamar funcao para inserir em ordem na lista
void preencherLista(unsigned long long tabela[], Lista *lista){
    no *novoNo;
    for (int i = 0; i < 256; i++){
        if(tabela[i] > 0){
            novoNo = malloc(sizeof(*novoNo));
            if(novoNo){
                novoNo->caracter = i;
                novoNo->frequencia = tabela[i];
                novoNo->dir = NULL;
                novoNo->esq = NULL;
                novoNo->prox = NULL;

                inserirListaOrdenadamente(lista, novoNo);

            } else{
                printf("Erro ao alocar memória preenchendo a lista.\n");
                break;
            }
        }
    }
}

void lerArquivoETabelaFrequencia(char nomeArquivo[], unsigned long long frequencia[]){
    FILE* arquivo = fopen(nomeArquivo, "rb");

    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    unsigned long long bytes_totais = 0;

    int byte;
    while((byte = fgetc(arquivo)) != EOF){
        frequencia[(unsigned char)byte]++;
        bytes_totais++;
    }

    if(ferror(arquivo)){
        printf("Erro na leitura do arquivo.\n");
        fclose(arquivo);
        exit(1);
    }

    fclose(arquivo);
    /*
    imprimir a tabela:
    printf("Total de bytes: %llu\n", bytes_totais);
    printf("Frequencias encontradas: \n");

    for(int i= 0; i < 256; i++){
        if(frequencia[i] != 0){
            printf("Byte %d: %llu vezes\n", i, frequencia[i]);
        }    
    }
    */
    return;
}

void removerNoInicio(Lista *lista, no **removido){
    *removido = NULL;

    if(lista->inicio != NULL){
        *removido = lista->inicio;
        lista->inicio = lista->inicio->prox;
        (*removido)->prox = NULL;
        lista->tamanho--;
    }
}

//remover Nós da lista para montar a arvore
void gerarArvore(Lista *lista){
    no *primeiro, *segundo, *novo;

    while(lista->tamanho > 1){
        primeiro = NULL;
        segundo = NULL;

        removerNoInicio(lista, &primeiro);
        removerNoInicio(lista, &segundo);

        if(primeiro == NULL || segundo == NULL){
            printf("Erro ao remover nos da lista.\n");
            return;
        }

        novo = malloc(sizeof(*novo));

        if(novo != NULL){
            novo->caracter = '+';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->prox = NULL;

            inserirListaOrdenadamente(lista, novo);
        } else{
            printf("Erro na alocaçao de memoria em gerar arvore\n");

            inserirListaOrdenadamente(lista, primeiro);
            inserirListaOrdenadamente(lista, segundo);
            return;
        }
    }
}

void alturaArvore(no *raiz, int *altura){
    int esq = 0, dir = 0;

    if(raiz == NULL){
        *altura = 0;
        return;
    }
    alturaArvore(raiz->dir, &dir);
    alturaArvore(raiz->esq, &esq);

    if(esq > dir){
        *altura = esq + 1;
    } else{
        *altura = dir + 1;
    }
}

//estudar
void alocarMemoriaHuffman(unsigned char **bits, int quantidadeBits){
    int quantidadeBytes = (quantidadeBits + 7)/8;

    *bits = calloc(quantidadeBytes, sizeof(unsigned char)); //calloc inicializa bytes com 0

    if(*bits == NULL){
        printf("Erro ao alocar memoria do huff.\n");
    }
}

//estudar
void gerarHuffman(char tabelaHuffman[][256], no *raiz, char *caminho, int colunas){
    char esq[colunas], dir[colunas];
    
    if(raiz->esq== NULL && raiz->dir == NULL){//verifica se chegou numa folha
        strcpy(tabelaHuffman[raiz->caracter], caminho); //copia o caminho para aposicao do caracter na tabela
    } else{
        strcpy(esq, caminho);
        strcpy(dir, caminho);

        strcat(esq, "0");
        strcat(dir, "1");

        gerarHuffman(tabelaHuffman, raiz->esq, esq, colunas);
        gerarHuffman(tabelaHuffman, raiz->dir, dir, colunas);
    }
 
}

void imprimirHuffman(char tabelaHuffman[][256]){
    for(int i= 0; i<256; i++){
        if(strlen(tabelaHuffman[i]) >0){
            printf("\t%3d: %s\n", i, tabelaHuffman[i]);
        }
    }
}

int main(){
    char nomeArquivo[256];
    unsigned long long tabelaFrequencia[256] = {0};
    Lista listaBase;
    char tabelaHuffman[256][256] = {0};

    printf("Digite o nome do arquivo(incluindo terminação .png, .mp4, etc): \n");
    fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
    nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';

    lerArquivoETabelaFrequencia(nomeArquivo, tabelaFrequencia);

    iniciarLista(&listaBase);
    preencherLista(tabelaFrequencia, &listaBase);

    gerarArvore(&listaBase);

    if(listaBase.inicio == NULL){
        printf("Arquivo vazio ou sem dados para comprimir.\n");
        return 1;
    }

    //alocarMemoriaHuffman();
    
    int altura;
    alturaArvore(listaBase.inicio, &altura);
    int colunas = altura + 1;

    gerarHuffman(tabelaHuffman, listaBase.inicio, "", colunas);
    imprimirHuffman(tabelaHuffman);



    return 0;
}