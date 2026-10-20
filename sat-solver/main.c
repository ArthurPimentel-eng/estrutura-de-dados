#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct noLiteral{
    int valor;
    struct noLiteral *prox;
} noLiteral;

typedef struct listaLiterais{
    int tamanho;
    noLiteral *inicio;
    noLiteral *fim;
}listaLiterais;

typedef struct noClausulas{
    listaLiterais literais;
    struct noClausulas *prox;
 } noClausulas;

typedef struct listaClausulas{
    int tamanho;
    noClausulas *inicio;
    noClausulas *fim;
}listaClausulas;

typedef struct formula {
    int qntVariaveis;
    listaClausulas clausulas;
}formula;

void iniciarListaLiterais(listaLiterais *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

void iniciarListaClausulas(listaClausulas *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

void iniciarFormula(formula* f){
    f->qntVariaveis = 0;
    iniciarListaClausulas(&f->clausulas);
}

//inserçao no final 
void inserirLiteral(listaLiterais *lista, int valor){
    noLiteral *novo = malloc(sizeof(noLiteral));

    if(novo == NULL){
        printf("Erro ao alocar nó literal");
        return;
    }

    novo->valor = valor;
    novo->prox = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novo;
        lista->fim = novo;
    } else {
        lista->fim->prox = novo;
        lista->fim = novo;
    }
    lista->tamanho++;
}

//insercao no final
void inserirClausula(listaClausulas* lista){
    noClausulas* novo = malloc(sizeof(noClausulas));

    if(novo == NULL){
        printf("Erro ao alocar nó clausula.");
        return;
    }

    iniciarListaLiterais(&novo->literais);
    novo->prox = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novo;
        lista->fim = novo;
    } else {
        lista->fim->prox = novo;
        lista->fim = novo;
    }
    lista->tamanho++;
}

void lerClausulas(FILE *arquivo, listaClausulas *clausulas, int qntClausulas){
    int clausulasLidas = 0;
    int valor;

    if(qntClausulas > 0){
        inserirClausula(clausulas);
    }
    
    noClausulas *atualClausula = clausulas->inicio;
    
    while(clausulasLidas < qntClausulas){
        char c;
        //pula linhas em branco
        if (fscanf(arquivo, " %c", &c) != 1) break;

        if (c == 'c') {
            fscanf(arquivo, "%*[^\n]"); // pula a linha toda ate achar um \n
            continue;
        }
        //devolve o caracter p leitura normal se n for um c
        ungetc(c, arquivo);

        //se for numero, faz a leitura normalmente
        if (fscanf(arquivo, "%d", &valor) == 1) {
            if (valor == 0) {
                clausulasLidas++;
                if (clausulasLidas < qntClausulas) {
                    inserirClausula(clausulas);
                    atualClausula = atualClausula->prox;
                }
            } else {
                inserirLiteral(&atualClausula->literais, valor);
            }
        }
    }
}

void lerArquivo(FILE * arquivo, formula *f){
    char caracter;
    char formato[4];
    int qntClausulas;

    while(fscanf(arquivo, " %c", &caracter) == 1){
        if(caracter == 'c'){
            fscanf(arquivo, "%*[^\n]");
        } else if (caracter == 'p'){
            fscanf(arquivo, "%3s", formato);
            fscanf(arquivo, "%d %d", &f->qntVariaveis, &qntClausulas);

            iniciarListaClausulas(&f->clausulas);

            lerClausulas(arquivo, &f->clausulas, qntClausulas);
            break;
        }
    }
}

//FIM DA LEITURA DO SAT INICIO DA SOLUCAO DELE

//percorre os literais de uma cláusula e procura pelo menos um verdadeiro
int avaliarClausula(noClausulas *clausula, int atribuicao[]){
    noLiteral *atual = clausula->literais.inicio;
    
    while(atual != NULL){
        int variavel = abs(atual->valor);

        if(atual->valor > 0 && atribuicao[variavel] == 1){
            return 1;
        }

        if(atual->valor < 0 && atribuicao[variavel] == 0){
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

//percorre as cláusulas da fórmula e exige que todas sejam verdadeiras
int avaliarFormula(formula *f, int atribuicao[]){
    noClausulas *atual = f->clausulas.inicio;

    while(atual != NULL){
        if(avaliarClausula(atual, atribuicao) == 0){
            return 0;
        }
        atual = atual->prox;
    }

    return 1;
}

//a função testa a variável atual como 0, avança para a próxima; se não der certo, testa como 1.
int resolverSat(formula *f, int atribuicao[], int variavelAtual){
    if(variavelAtual > f->qntVariaveis){
        return avaliarFormula(f, atribuicao);
    }

        atribuicao[variavelAtual] = 0;
        if(resolverSat(f, atribuicao, variavelAtual + 1)) {
            return 1;
        }

        atribuicao[variavelAtual] = 1;
        if(resolverSat(f, atribuicao, variavelAtual + 1)) {
            return 1;
        }

        return 0;
        
    }

void destruirListaLiterais(listaLiterais *lista) {
    noLiteral *atual = lista->inicio;

    while (atual != NULL) {
        noLiteral *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

void destruirFormula(formula *f) {
    noClausulas *atual = f->clausulas.inicio;

    while (atual != NULL) {
        noClausulas *proxima = atual->prox;

        destruirListaLiterais(&atual->literais);

        free(atual);
        atual = proxima;
    }

    f->clausulas.inicio = NULL;
    f->clausulas.fim = NULL;
    f->clausulas.tamanho = 0;
    f->qntVariaveis = 0;
}


int main(){
    char nomeArquivo[200];
    printf("Digite o nome do arquivo(com extensão!): \n");
    scanf("%s", nomeArquivo);

    formula f;
    iniciarFormula(&f);

    FILE * arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL){
        printf("Erro ao abrir arquivo.");
        return 1;
    }

    lerArquivo(arquivo, &f);

    fclose(arquivo);
    //imprimirFormula(&f);

    // RESOLVER SAT
    //criacao do vetor, variaveis + 1 pq ignoramos a posiçao 0 do vetor;
    int *atribuicao = calloc(f.qntVariaveis + 1, sizeof(int));

    if (resolverSat(&f, atribuicao, 1) == 1){
        printf("\n\tSAT\n\n");
        for(int i = 1; i <= f.qntVariaveis; i++){
            printf("%d = %d\n", i, atribuicao[i]);
        }
    }else{
        printf("\nUNSAT!\n");
    }

    free(atribuicao);
    destruirFormula(&f);

    return 0;
}