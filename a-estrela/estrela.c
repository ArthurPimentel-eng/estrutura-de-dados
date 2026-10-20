#include <stdio.h>
#define maxn 100
#define INF 1000000000 //custo de um no ainda nao alcançado

int custo[maxn][maxn]; //custo da aresta x ate y
int h[maxn]; //heuristica de cada no

int main () {
    int n, m, dir, ini, fim; //n é o numero de nos, m é o numero de arestas, dir indica se o grafo é direcionado

    printf("Digite o numero de nos:\n");
    printf("Digite os valores heuristicos:\n");
    printf("Se quiser um grafo direcionado digite 1, se esse nao for o caso, digite 0:\n");
    printf("Digite o numero de arestas:\n");
    printf("Digite as origens, os destinos e os custos:\n");
    printf("Digite o inicio e o fim:\n");
    if(scanf("%d", &n) !=1 || n < 1 || n > maxn)
        return 1;


for(int i = 0; i < n; i++)        //vai ler os valores heurísticos. Ex: h[0] = 30, h[1] = 26...
    if(scanf("%d", &h[i]) != 1)
        return 1;

    if(scanf("%d", &dir) != 1)    //se dir for 1 é direcionado (só tem caminho de ida)
        return 1;

    if(scanf("%d", &m) != 1 || m < 0)
        return 1;

//le a origem , o destino e o custo de um no especifico ate outro (c = custo)
for(int k = 0; k < m; k++) {
int origem, destino, c;
    if(scanf("%d %d %d", &origem, &destino, &c) != 3 || origem < 0 || origem >= n || destino >= n || destino < 0 || c <= 0)
        return 1;
    custo[origem][destino] = c;
    if(dir != 1)
        custo[destino][origem] = c;
}

//le o ponto de partida (inicio) e o objetivo (fim)
    if(scanf("%d %d", &ini, &fim) != 2 || ini < 0 || ini >= n || fim < 0 || fim >= n)
        return 1;

//preparar os nos, ou "zera"
int g[maxn], pai[maxn], aberto[maxn];
for(int i = 0; i < n; i++) {
    g[i] = INF;
    pai[i] = -1;
    aberto[i] = 0;
}

g[ini] = 0;
aberto[ini] = 1;

//escolher dentro dos nos abertos o melhor
for(;;) {
int atual = -1;
int f_atual = 0;
for(int i = 0; i < n; i++) {
    if(!aberto[i]) continue;
    int f = g[i] + h[i];
    if(atual == -1 || f < f_atual) {
        atual = i;
        f_atual = f;
    }
}

    if(atual == -1) break;
    if(atual == fim) break;

    aberto[atual] = 0;

//testa os caminhos do no escolhido ate os vizinhos e atualiza o custo g se achar um caminho melhor
    for(int vizinho = 0; vizinho < n; vizinho++) {
        if(custo[atual][vizinho] == 0) continue;
int novo = g[atual] + custo[atual][vizinho];
        if(novo < g[vizinho]) {
            g[vizinho] = novo;
            pai[vizinho] = atual;
            aberto[vizinho] = 1;

        }
    }

}

//se g[fim] ainda é INF nao chegamos no nó
    if(g[fim] == INF) {
        printf("Nao existe caminho de %d ate %d\n", ini, fim);
        return 0;
    }

//reconstroi o caminho de tras pra frente e imprime
int caminho[maxn], tam = 0;
for(int v = fim; v != -1; v = pai[v])
    caminho[tam++] = v;

//coloca o caminho na ordem certa de novo e imprime
    printf("Caminho: ");
for(int i = tam -1; i >= 0; i--)
    printf("%c ", 'A' + caminho[i]);
    printf("\nCusto: %d\n", g[fim]); //imprime o custo tambem

    return 0;
}