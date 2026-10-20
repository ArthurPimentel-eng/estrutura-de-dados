#include <stdio.h>
#include<stdlib.h>
#include<string.h>


typedef struct no{
    void *caracter;
    void *frequencia;
    struct no *esq, *dir, *prox; 
}No;

typedef struct lista{
    void* tamanho;
    No *inicio;
}Lista;

void iniciarLista(Lista *lista){
    lista->inicio = NULL;
    lista->tamanho = malloc(sizeof(unsigned long long));

    if(lista->tamanho != NULL){
        *(unsigned long long *)lista->tamanho = 0;
    }
}

void inserirOrdenadoLista(Lista *lista, No *novoNo){
    No *aux;
    if(lista->inicio == NULL){
        lista->inicio = novoNo;
    } else if(*(unsigned long long*)lista->inicio->frequencia > *(unsigned long long*)novoNo->frequencia){
        novoNo->prox = lista->inicio;
        lista->inicio = novoNo;
    }else{
        aux = lista->inicio;
        while(aux->prox != NULL && *(unsigned long long*)aux->prox->frequencia <= *(unsigned long long*)novoNo->frequencia){
            aux = aux->prox;
        }
        novoNo->prox = aux->prox;
        aux->prox = novoNo;
    }
    (*(unsigned long long*)lista->tamanho)++;
    return;
}

/**
 * Pega cada elemento da tabela pra preencher a lista(ordenadamente usando a função acima)
 */
void preencherLista(unsigned long long tabela[], Lista* lista){
    No *novoNo;
    for(int i=0; i < 256; i++){
        if(tabela[i] > 0){
            novoNo = malloc(sizeof(No));

            if(novoNo == NULL){
                printf("Erro ao alocar memoria em preencherLista!\n");
                return;
            }

            novoNo->caracter = malloc(sizeof(int));
            novoNo->frequencia = malloc(sizeof(unsigned long long));
            if(novoNo != NULL){
                *(int*)novoNo->caracter = i;
                *(unsigned long long*)novoNo->frequencia = tabela[i];
                novoNo->dir = NULL;
                novoNo->esq = NULL;
                novoNo->prox = NULL;

                inserirOrdenadoLista(lista, novoNo);    
            } 
        }
    }
}

/**
 * Abrir arquivo e montar a tabela de quantas vezes cada um deles aparece
 */
void montarTabelaFrequencia(FILE *arquivo, unsigned long long tabelaFrequencia[256]){

    if(arquivo == NULL){
        printf("Falha ao abrir arquivo!");
        exit(1);
    }

    if(ferror(arquivo)){
        printf("Falha na leitura do arquivo!");
        exit(1);
    }

    unsigned long long bytesTotais = 0;
    int byte;

    while((byte = fgetc(arquivo)) != EOF){
        tabelaFrequencia[(unsigned char)byte]++;
        bytesTotais++;
    }
}

/**
 * Necessário pra criação da árvore de Huffman
 */
No* removerNoInicio(Lista *lista){
    No *aux = NULL;

    if(lista->inicio != NULL){
        aux = lista->inicio;
        lista->inicio = aux->prox;
        aux->prox = NULL;
        (*(unsigned long long*)lista->tamanho)--;
    }

    return aux;
}

/**
 * Remove dois primeiros nós da lista, cria um novo nó e faz esquerda e direita apontarem para os nós retirados
 */
No* gerarArvore(Lista* lista){
    No *primeiro, *segundo, *novoNo;
    while((*(unsigned long long*)lista->tamanho) > 1){
        primeiro = removerNoInicio(lista);
        segundo = removerNoInicio(lista);

        novoNo = malloc(sizeof(No));

        //caso a alocacao de memoria falhe, recuperamos os nós retirados
        if(novoNo == NULL){
            printf("Erro ao alocar memoria em gerarArvore!\n");
            inserirOrdenadoLista(lista, primeiro);
            inserirOrdenadoLista(lista, segundo);
            return NULL;
        }

        novoNo->caracter = NULL;
        novoNo->frequencia = malloc(sizeof(unsigned long long));

        //caso a alocacao de memoria falhe, recuperamos os nós retirados
        if(novoNo->frequencia == NULL){
            free(novoNo);
            inserirOrdenadoLista(lista, primeiro);
            inserirOrdenadoLista(lista, segundo);
            printf("Erro ao alocar memoria para frequencia!\n");
            return NULL;
        }

        *(unsigned long long*)novoNo->frequencia =  *(unsigned long long*)primeiro->frequencia +  *(unsigned long long*)segundo->frequencia;
        novoNo->esq = primeiro;
        novoNo->dir = segundo;
        novoNo->prox = NULL;

        inserirOrdenadoLista(lista, novoNo);
    }
    return lista->inicio;
}

/**
 * Quantidade de arestas da árvore
 */
int alturaArvore(No* raiz){
    int esq, dir;

    if(raiz == NULL){
        return -1;
    } else{
        esq = alturaArvore(raiz->esq) + 1;
        dir = alturaArvore(raiz->dir) + 1;    

        if(esq > dir){
            return esq;
        } else{
            return dir;
        }
    }
}

/**
 * Cria uma matriz dinamica para o dicionario
 */
char** alocarDicionario(int alturaColunas){ 
    char **dicionario;

    dicionario = malloc(sizeof(char*)* 256); 

    if(dicionario == NULL){
        printf("Erro ao alocar dicionario.");
        exit(1);
    }

    for(int i = 0; i<256; i++){
        dicionario[i]= calloc(alturaColunas + 1, sizeof(char)); //calloc limpa a memoria antes de alocar
    }

    return dicionario;
}

/**
 * Preenche o dicionario com o codigo de cada aresta
 */
void gerarDicionario(char** dicionario, No* raiz, char *codigo, int colunas){
    if(raiz == NULL){
        return;
    }

    if(raiz->dir == NULL && raiz->esq == NULL){
        int caracter = *(int *)raiz->caracter;

        if(colunas == 0){
            strcpy(dicionario[caracter], "0"); 
            return;
        }else{
            codigo[colunas] = '\0'; 
            strcpy(dicionario[caracter], codigo);
            return;
        }
    }

    codigo[colunas] = '0';
    gerarDicionario(dicionario, raiz->esq, codigo, colunas + 1); 

    codigo[colunas] = '1';
    gerarDicionario(dicionario, raiz->dir, codigo, colunas + 1);
}

/**
 * Salvando a árvore para o cabeçalho
 */
void salvarPreOrdem(FILE *arquivo, No* raiz, unsigned int *qntBytes){
    if(raiz == NULL){
        return;
    }

    if(raiz->esq == NULL && raiz->dir == NULL){
        int caracter = *(int*)raiz->caracter;

        if(caracter == '*' || caracter == '\\'){
            fputc('\\', arquivo);
            (*qntBytes)++;
        }
        fputc(caracter, arquivo);
        (*qntBytes)++;
    }else{
        fputc('*', arquivo);  
        (*qntBytes)++;

        salvarPreOrdem(arquivo, raiz->esq, qntBytes);
        salvarPreOrdem(arquivo, raiz->dir, qntBytes);
    }
    return;
}

/**
 * coloca a nova codificaçao no arquivo.huff e retorna o lixo que precisamos pro cabeçalho
 */
int compactar(char** dicionario, FILE *entrada, FILE *saida){
    unsigned char guardadorDeBits = 0;
    int quantidadeBits = 0;
    int caracter;

    while((caracter = fgetc(entrada)) != EOF){
        char *codigo = dicionario[(unsigned char)caracter]; 

        for(int i = 0; codigo[i] != '\0'; i++){
            guardadorDeBits = (guardadorDeBits << 1) | (codigo[i] == '1'); 
            quantidadeBits++;

            if(quantidadeBits == 8){
                fputc(guardadorDeBits, saida);
                guardadorDeBits = 0;
                quantidadeBits = 0;
            }
        }
    }

    int lixo = 0;
    if(quantidadeBits > 0){
        lixo = 8 - quantidadeBits;
        guardadorDeBits = guardadorDeBits << lixo;   //REVISAR GARANTE QUE O LIXO ESTEJA NO final DO BYTE   
        fputc(guardadorDeBits, saida);
    }

    return lixo;    
}

/**
 * junta o tamanho da arvore, o lixo no cabeçalho do arquivo.huff
 */
void montarCabecalho(unsigned int tamanhoArvore, int lixo, FILE *saida){
    unsigned char cabecalho[2];

    cabecalho[0] = (lixo << 5) | (tamanhoArvore >> 8);        //REVISAR
    cabecalho[1] = tamanhoArvore & 255;   //REVISAR 

    fseek(saida, 0, SEEK_SET); 

    fputc(cabecalho[0], saida);
    fputc(cabecalho[1], saida);
    return;
}

/**
 * Recria a arvore para a descompactaçao
 */
No* refazerArvore(FILE* arquivo, unsigned int *tamanhoArvore){

    if(*tamanhoArvore == 0){
        return NULL;
    }

    int byte = fgetc(arquivo);
    (*tamanhoArvore)--;

    No *novoNo = malloc(sizeof(No));

    if(novoNo == NULL){
        printf("ERRO AO ALOCAR NÓ EM REFAZER ARVORE.");
        return NULL;
    }

    novoNo->caracter = malloc(sizeof(int));
    novoNo->dir = NULL;
    novoNo->esq = NULL;
    novoNo->frequencia = NULL;
    novoNo->prox = NULL;

    if(byte == '\\'){ 
        byte = fgetc(arquivo);
        (*tamanhoArvore)--;
        *(int*)novoNo->caracter = byte;
    } else if(byte == '*'){
        *(int*)novoNo->caracter = '*';
        novoNo->esq = refazerArvore(arquivo, tamanhoArvore);
        novoNo->dir = refazerArvore(arquivo, tamanhoArvore);
        return novoNo;
    }else{
        *(int*)novoNo->caracter = byte;
    }
    return novoNo;
}

/**
 * Cria um novo arquivo descompactado
 */
void descompactar(FILE *entrada, FILE *saida, No *raiz, int lixo){
    No *atual = raiz;
    int byteAtual = fgetc(entrada);
    int byteSeguinte = fgetc(entrada);

    int limiteBitsLidos;

    while (byteAtual !=EOF){
        
        if(byteSeguinte == EOF){
            limiteBitsLidos = 8 - lixo;
        } else{
            limiteBitsLidos = 8;
        }
    

        for(int i=7; i >= (8 - limiteBitsLidos); i--){
            int bit = (byteAtual >> i) & 1; 

            if(bit == 0){
                atual = atual->esq;
            }else{
                atual = atual->dir;
            }

            if(atual->esq == NULL && atual->dir == NULL){
                fputc(*(int*)atual->caracter, saida);
                atual = raiz;
            }
        }
        
        byteAtual = byteSeguinte;
        byteSeguinte = fgetc(entrada);
    }
    return;
}

void destruirArvore(No *raiz){
    if(raiz == NULL){
        return;
    }

    destruirArvore(raiz->esq);
    destruirArvore(raiz->dir);

    free(raiz->caracter);
    free(raiz->frequencia);
    free(raiz);
}


void destruirDicionario(char **dicionario){
    if(dicionario == NULL){
        return;
    }

    for(int i = 0; i < 256; i++){
        free(dicionario[i]);
    }

    free(dicionario);
}



int main(){
    int decisao;
    printf("Você gostaria de (digite o número):\n1. Compactar um arquivo.\n2. Descompactar um arquivo\n3.Compactar e descompactar um arquivo.\n");
    scanf("%d", &decisao);


    /**
    * COMPACTAR
    */
    if (decisao == 1 || decisao == 3) {
        char nomeArquivo[200];
        unsigned long long tabelaFrequencia[256] = {0};
        Lista listaBase;
        No* arvore;

        printf("Digite o nome do arquivo a ser compactado, incluindo extensao (.pdf, .jpeg, .mp4...):\n");
        scanf("%s", nomeArquivo);

        FILE *entrada = fopen(nomeArquivo, "rb");
        if (entrada == NULL) {
            printf("Erro ao abrir o arquivo de entrada para compactacao!\n");
            return 1;
        }

        FILE *saida = fopen("arquivo.huff", "wb");
        if (saida == NULL) {
            printf("Erro ao criar o arquivo codificado.huff!\n");
            fclose(entrada);
            return 1;
        }

        montarTabelaFrequencia(entrada, tabelaFrequencia);

        iniciarLista(&listaBase);
        preencherLista(tabelaFrequencia, &listaBase);

        arvore = gerarArvore(&listaBase);
        if (arvore != NULL) {
            printf("\n\tARVORE\n\n");
            imprimirArvore(arvore, 0);
        } else {
            printf("Erro ao construir a arvore\n");
            fclose(entrada);
            fclose(saida);
            return 1;
        }

        int colunas = alturaArvore(arvore);
        char** dicionario;
        char codigo[256] = {0};

        dicionario = alocarDicionario(colunas);
        gerarDicionario(dicionario, arvore, codigo, 0);

        rewind(entrada);
        unsigned int tamanhoArvoreEmNos = 0;

        // Montagem do cabeçalho
        // Reservar espaço para os dois primeiros bytes (lixo e tamanho da arvore)
        fputc(0, saida); 
        fputc(0, saida);

        salvarPreOrdem(saida, arvore, &tamanhoArvoreEmNos);
        int lixo = compactar(dicionario, entrada, saida);
        montarCabecalho(tamanhoArvoreEmNos, lixo, saida);

        fclose(entrada);
        fclose(saida);

        destruirDicionario(dicionario);
        destruirArvore(arvore);
        free(listaBase.tamanho);

    printf("\n\tARQUIVO COMPACTADO\n\n");
    }

    /**
     * DESCOMPACTAR
    */
    if (decisao == 2 || decisao == 3) {
        char nomeDescompactado[200];

        printf("Digite o nome para o arquivo descompactado (com extensao): \n");
        scanf("%s", nomeDescompactado);

        FILE *arquivoHuff = fopen("arquivo.huff", "rb");
        FILE *arquivoDescompactado = fopen(nomeDescompactado, "wb");

        if (arquivoHuff == NULL || arquivoDescompactado == NULL) {
            printf("Erro ao abrir arquivos para descompactacao!\n");
            return 1;
        }

        unsigned char cabecalho[2];
        cabecalho[0] = fgetc(arquivoHuff);
        cabecalho[1] = fgetc(arquivoHuff);

        int lixoExtraido = cabecalho[0] >> 5;  
        unsigned int tamanhoArvoreExtraida = ((cabecalho[0] & 31) << 8) | cabecalho[1]; 

        No *arvoreReconstruida = refazerArvore(arquivoHuff, &tamanhoArvoreExtraida);
        descompactar(arquivoHuff, arquivoDescompactado, arvoreReconstruida, lixoExtraido);

        fclose(arquivoHuff);
        fclose(arquivoDescompactado);

        destruirArvore(arvoreReconstruida);

    printf("ARQUIVO DESCOMPACTADO!");
    }

    if (decisao < 1 || decisao > 3) {
        printf("Opcao invalida!\n");
    }

    return 0;
}