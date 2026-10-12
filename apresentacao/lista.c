#include <stdio.h>
#include <stdlib.h>

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

void inserirmeio(No **lista, int num, int ant) {
    No *aux, *novo = malloc(sizeof(No));

    if(novo) {
        novo->valor = num;
        //é o primeiro?
        if(*lista == NULL) {
            novo->proximo = NULL;
            *lista = novo;
        }
        else
        aux = *lista;
        while(aux->valor != ant && aux->proximo)
            aux = aux->proximo;
        novo->proximo = aux->proximo;
        aux->proximo = novo;

        
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



int main () {
    int opcao, valor, anterior;
    No *lista = NULL;

    do{
        printf("0 - sair\n1 - inserirI\n2 - inserirF\n3 - inserirM\n4 - imprimir\n");
        scanf("%d", &opcao);

        switch(opcao) {
        case 1:
            printf("Digite um valor: ");
            scanf("%d", &valor);
            inseririnicio(&lista, valor);
                break;
        case 2:
            printf("Digite um valor: ");
            scanf("%d" , &valor);
            inserirfim(&lista, valor);
                break;
        case 3:
            printf("Digite um valor e o valor de referencia: ");
            scanf("%d %d", &valor, &anterior);
            inserirmeio(&lista, valor, anterior);
                break;
        case 4:
            imprimir(lista);
            break;
        default: 
        if(opcao != 0) {
            printf("Opcao invalida\n");
        }
    }
    
}while(opcao != 0);

    return 0;
} 