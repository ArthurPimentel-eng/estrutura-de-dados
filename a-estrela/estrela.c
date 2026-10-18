#include <stdio.h>
#define maxn 100
#define INF 1000000000

int custo[maxn][maxn];
int h[maxn]; //heuristica de cada no

int main () {
    int n, m, dir, ini, fim; //n é o numero de nos, m é o numero de arestas

    printf("Digite o numero de nos:\n");
    printf("Digite os valores heuristicos:\n");
    printf("Se quiser um grafo direcionado digite 1, se esse nao for o caso, digite 0:\n");
    printf("Digite o numero de arestas:\n");
    printf("Digite as origens, os destinos e os custos:\n");
    printf("Digite o inicio e o fim:\n");
    if(scanf("%d", &n) !=1 || n < 1 || n > maxn)
        return 1;


for(int i = 0; i < n; i++)        //vai ler os valores. Ex: h[0] = 30, h[1] = 26...
    if(scanf("%d", &h[i]) != 1)
        return 1;

    if(scanf("%d", &dir) != 1)    //se dir for 1 é direcionado (só tem caminho de ida)
        return 1;

    if(scanf("%d", &m) != 1 || m < 0)
        return 1;


for(int k = 0; k < m; k++) {
int o, d, c;
    if(scanf("%d %d %d", &o, &d, &c) != 3 || o < 0 || o >= n || d >= n || c <= 0)
        return 1;
    custo[o][d] = c;
    if(dir != 1)
        custo[d][o] = c;
}


    if(scanf("%d %d", &ini, &fim) != 2 || ini < 0 || ini >= n || fim < 0 || fim >= n)
        return 1;

int g[maxn], pai[maxn], aberto[maxn];
for(int i = 0; i < n; i++) {
    g[i] = INF;
    pai[i] = -1;
    aberto[i] = 0;
}

g[ini] = 0;
aberto[ini] = 1;

for(;;) {
int atual = -1;
    for(int i = 0; i < n; i++)
        if(aberto[i] && (atual == -1 || g[i] + h[i] < g[atual] + h[atual]))
        atual = i;

    if(atual == -1) break;
    if(atual == fim) break;

    aberto[atual] = 0;

    for(int v = 0; v < n; v++) {
        if(custo[atual][v] == 0) continue;
int novo = g[atual] + custo[atual][v];
        if(novo < g[v]) {
            g[v] = novo;
            pai[v] = atual;
            aberto[v] = 1;

        }
    }

}

    if(g[fim] == INF) {
        printf("Nao existe caminho de %d ate %d\n", ini, fim);
        return 0;
    }

int caminho[maxn], tam = 0;
for(int v = fim; v != -1; v = pai[v])
    caminho[tam++] = v;

    printf("Caminho: ");
for(int i = tam -1; i >= 0; i--)
    printf("%c ", 'A' + caminho[i]);
    printf("\nCusto: %d\n", g[fim]);


}