#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_ALEATORIO 1000000
#define ARQUIVO_BENCHMARK "resultados.csv"

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

Noarv* buscararv(Noarv *raiz, int num, int *passagens) {
    if(raiz == NULL)
        return NULL;

    (*passagens)++;
    if(num == raiz->valor)
        return raiz;
    else if(num < raiz->valor) 
        return buscararv(raiz->esq, num, passagens);
    return buscararv(raiz->dir, num, passagens);   
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

void liberarlista(Neo* lista)
{
    if(lista == NULL) return;
    liberarlista(lista->proximo);
    free(lista);
}

void liberararvore(Noarv* raiz)
{
    if(raiz == NULL) return;
    liberararvore(raiz->esq);
    liberararvore(raiz->dir);
    free(raiz);
}

void executarbenchmark(FILE *saida, int tamanhomax, int passo, int repeticoes, int buscas)
{
    int tamanho, repeticao, busca, alvo, passagenslista, passagensarv;
    long totallista, totalarv;
    No* lista;
    Noarv* raiz;

    fprintf(saida, "estrutura,tamanho,repeticao,media_passagens\n");

    for(tamanho = passo; tamanho <= tamanhomax; tamanho += passo)
    {
        for(repeticao = 1; repeticao <= repeticoes; repeticao++)
        {
            lista = NULL;
            raiz = NULL;
            gerarAleatorios(&lista, &raiz, tamanho);

            totallista = 0;
            totalarv = 0;

            for(busca = 0; busca < buscas; busca++)
            {
                alvo = rand() % MAX_ALEATORIO;
                passagenarv = 0;
                buscar(&lista, alvo, &passagenslista);
                buscararv(raiz, alvo, &passagensarv);
                totallista += passagenslista;
                totalarv += totalarv;
            }

            fprintf(saida, "lista,%d,%d,%.2f\n", tamanho, repeticao, (double) totallista / buscas);
            fprintf(saida, "arvore,%d,%d,%.2f\n", tamanho, repeticao, (double) totalarv / buscas);
            
            liberarlista(lista);
            liberararvore(raiz);
        }
    }
}



int main () {
    
    int opcao, valor, anterior, passagens, quantidade, passagensA;
    int tamanhomax, passo ,repeticoes, buscas;
    FILE* saida;
    No *lista = NULL, *elemento;
    Noarv *busca, *raiz = NULL;
    srand(time(NULL));

    do{
        printf("0 - sair\n1 - inserirI\n2 - inserirF\n3 - imprimir lista\n4 - buscar na lista\n5 - gerar numeros aleatorios e inserir\n6 - inserir na arvore\n7 - imprimir arvore\n8 - buscar na arvore\n9 - Arquivo da Plotagem\n");
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
    passagensA = 0;
    busca = buscararv(raiz, valor, &passagensA);
    if(busca)
    printf("\nValor encontrado: %d\nPassagens até encontrar: %d\n", busca->valor, passagensA);
    else
    printf("Valor nao encontrado\nPassagen feitas: %d\n", passagensA);
    break;

case 9:
    printf("Insira o Tamanho Maximo das Estruturas, Passos, Repeticoes e buscas: ")
    if(scanf("%d %d %d %d", &tamanhomax, &passo, &repeticoes, &buscas) == 4 && tamanhomax > 0 && passo > 0 && repeticoes > 0 && buscas > 0)
    {
        arquivo = fopen(ARQUIVO_BENCHMARK, "w");
            if(arquivo) {
                executarbenchmark(arquivo, tamanhomax, passo, repeticoes, buscas);
                fclose(arquivo);
                printf("\nBenchmark salvo em %s\n\n", ARQUIVO_BENCHMARK);
            }
        else
           printf("\nNao foi possivel criar o arquivo\n\n");
    }
    else
        printf("\nValores invalidos\n\n");
    break;

default:
if(opcao != 0) {
    printf("Opcao invalida\n");
}
}

} while(opcao != 0);

liberarlista(lista);
liberararvore(raiz);

return 0;
}