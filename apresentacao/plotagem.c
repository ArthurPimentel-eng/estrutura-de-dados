#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_ALEATORIO 100000 

typedef struct no{
    int valor;
    struct no *proximo;
}No;

void inseririnicio(No **lista, int num) {
    No *novo = malloc(sizeof(No));

if(novo) {
    novo->valor = num;
    novo->proximo = *lista;
    *lista = novo;
}
else 
    printf("Erro\n");
}

void inserirfim(No **lista, int num) {
    No *aux, *novo = malloc(sizeof(No));

    if(novo) {
        novo->valor = num;
        novo->proximo = NULL;
        //é o primeiro?
        if(*lista == NULL) {
            *lista = novo;
        }
        else {
            aux = *lista;
        while(aux->proximo)
            aux = aux->proximo;
        aux->proximo = novo; }
    }
    else 
        printf("Erro\n");

}

void imprimir(No *no) {
    printf("\nLista:");
    while(no) {
        printf("%d ", no->valor);
        no = no->proximo;
    }
    printf("\n\n");
}


No* buscar(No **lista, int num, int *passagens) {
    No *aux = *lista;
    *passagens = 0;

    while(aux) {
        (*passagens)++;              
        if(aux->valor == num)
            return aux;              
        aux = aux->proximo;
    }
    return NULL;                     
}


//arvore

typedef struct noe{
    int valor;
    struct noe *esq, *dir;
}Noarv;

void inserirarv(Noarv **raiz, int num) {
    Noarv *aux = *raiz;
    while(aux){
        if(num < aux->valor) {
            raiz = &aux->esq;
        }
        else {
            raiz = &aux->dir;
        }
        aux = *raiz;
    }
    aux = malloc(sizeof(Noarv));
    aux->valor = num;
    aux->esq = NULL;
    aux->dir = NULL;
    *raiz = aux;
}

Noarv* buscararv(Noarv *raiz, int num) {
    if(raiz){
        if(num == raiz->valor){
            return raiz;}

        else if(num < raiz->valor) 
            buscararv(raiz->esq, num);
        else
            buscararv(raiz->dir, num);   
    }
    return NULL;
}

void imprimirarv(Noarv *raiz) {
    if(raiz){
        imprimirarv(raiz->esq);
        printf("%d ", raiz->valor);
        imprimirarv(raiz->dir);
    }
}

void gerarAleatorios(No **lista, Noarv **raiz, int qtd) {
    int i, num;
    for(i = 0; i < qtd; i++) {
        num = rand() % MAX_ALEATORIO;
        inseririnicio(lista, num);
        inserirarv(raiz, num);
    }
}


int main () {
    int opcao, valor, anterior, passagens, quantidade, passagensA;
    No *lista = NULL, *elemento;
    Noarv *busca, *raiz = NULL;
    srand(time(NULL));

    do{
        printf("0 - sair\n1 - inserirI\n2 - inserirF\n3 - imprimir lista\n4 - buscar na lista\n5 - gerar numeros aleatorios e inserir\n6 - inserir na arvore\n7 - imprimir arvore\n8 - buscar na arvore\n");
        scanf("%d", &opcao);

        switch(opcao) {
        case 1:
            printf("Digite os valores (-1 para terminar): ");
            for(; scanf("%d", &valor) == 1 && valor != -1; )
                inseririnicio(&lista, valor);
            break;
        case 2:
            printf("Digite os valores (-1 para terminar): ");
            for(; scanf("%d", &valor) == 1 && valor != -1; )
                inserirfim(&lista, valor);
            break;
        
        case 3:
            imprimir(lista);
            break;
        case 4:
            printf("Digite o valor a ser buscado: ");
            scanf("%d", &valor);
            elemento = buscar(&lista, valor, &passagens);
            if(elemento)
                printf("\nElemento encontrado: %d (%d passagens)\n", elemento->valor, passagens);
            else
                printf("\nValor nao encontrado (%d passagens)\n", passagens);
            break;
        case 5:
            printf("Digite a quantidade de elementos da lista:\n");
            if(scanf("%d", &quantidade) == 1 && quantidade > 0) {
                gerarAleatorios(&lista, &raiz, quantidade);
                printf("\n%d numeros gerados.\n\n", quantidade);
            }
            else
                printf("Quantidade invalida\n");
            break;
        case 6:
            printf("\nInsira os valores: ");
            for(; scanf("%d", &valor) == 1 && valor != -1; )
                inserirarv(&raiz, valor);
            break;
        case 7:
            printf("\nArvore:");
            imprimirarv(raiz);
            printf("\n\n");
            break;
        case 8:
            printf("\nDigite o valor a ser buscado: ");
            scanf("%d", &valor);
            busca = buscararv(raiz, valor);
            if(busca)
                printf("\nValor encontrado: %d\n", busca->valor);
            else
                printf("Valor nao encontrado\n");
            break;

        default:
            if(opcao != 0) {
                printf("Opcao invalida\n");
            }
        }

    } while(opcao != 0);

    return 0;
}