/**
 * @file main.c
 * @brief Implementação de um Árvore de Decisão com Entropia.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_CHAR 1024
#define MAX_HEIGHT 5

typedef struct
{
    char* name; ///< Nome do Atributo
    char** refs; ///< Array com os nome dos valores pode assumir.
    int nvalores; ///< Quantidade dos valores possíveis do atributo.
    int maximo; ///< Tamanho máximo do Array refs.
}tabela;

typedef struct no
{
    int atributo; ///< Atributo por qual dividimos a árvore.
    int classe; ///< Classe majoritária do nó.
    int nfilhos; ///< Quantidade de valores que o atributo pode assumir.
    struct no** filhos; ///< Array de ponteiros com os filhos do nó.
} no;



/**
* @brief Cria um novo nó folha.
*
* @param[in] classe Índice da classe majoritária desta folha.
* @return Ponteiro para a nova folha alocada com malloc.
*/
no* nova_folha(int classe)
{
    no* folha = malloc(sizeof(no));

    folha->atributo = -1;
    folha->classe = classe;
    folha->nfilhos = 0;
    folha->filhos = NULL;
    
    return folha;
}

/**
* @brief Cria um nó interno da árvore com todos os filhos inicializados com NULL.
*
* @param[in] classe Índice da classe majoritária deste nó.
* @param[in] nfilhos Quantidade de filhos do nó.
* @param[in] atributo Índice da coluna usada para dividir o nó.
* @return Ponteiro para o novo nó alocado com malloc.
*/
no* novo_no(int classe, int nfilhos, int atributo)
{
    no* interno = malloc(sizeof(no));
    
    interno->atributo = atributo;
    interno->classe = classe;
    interno->nfilhos = nfilhos;
    interno->filhos = calloc(nfilhos, sizeof(no*));
    
    return interno;
}


/**
* @brief Liberação de memória da matriz.
*
* Se a coluna não existe, ela será inicializada com malloc e com o token atual como seu nome.
* Se já existe o token será tratado como um valor da coluna, se já não houver um valor correspondente.
* O vetor @c refs cresce sob demanda (4 posições iniciais, depois +2).
*
* @param[in,out] t Tabela de tradução que receberá os tokens com os valores dos atributos.
* @param[in] token String com o valor do atributo que será inicializado na tabela.
* @param[in] ind Índice da coluna.
*/
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
            int novo;
            if(antigo) novo = antigo + 2;
            else novo = 4;

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

/**
 * @brief Transforma um valor da tabela no inteiro que o representa.
 * 
 * Os índices usados são os índices de @c refs, oque facilita o cálculo do
 * Ganho de informação.
 *
 * @param[in] trad Tabela usada para traduzir a string ao seu inteiro correspondente.
 * @param[in] token Valor da tabela que será traduzido.
 * @param[in] coluna Coluna de onde a string pertence.
 * @return Retorna o inteiro correspondente.
 */
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

/**
 * @brief Cálculo da Entropia do Atributo.
 * 
 * H(s) = - Somatório(pi log2(pi)).
 *
 * Classes com contagem zero são ignoradas por convenção.
 *
 * @param[in] cont Contagem das quantidades das diferentes classes.
 * @param[in] total Quantidade total de classes do atributo desejado disponíveis.
 * @param[in] num_classes Quantidade de classes disponíveis.
 * @return Retorna a entropia do atributo para o cálculo do ganho de informação.
 */
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

/**
 * @brief Criação do Vetor @p ramos que determina as linhas de cada valor do atributo escolhido.
 * 
 * Para cada valor possível do atributo será criado um vetor que define os índices das linhas 
 * de @p linhas que possuem este valor na coluna @p coluna.
 * 
 * @param[in] matriz Matriz traduzida que será usada para descobrir as linhas correspondentes de cada valor.
 * @param[in] linhas Índices das linhas disponíveis do subconjunto atual.
 * @param[in] sublinhas Quantidade de elementos em @p linhas.
 * @param[in] coluna Atributo usado para a divisão.
 * @param[in] natributos Quantidade de atributos possíveis da colunas.
 * @param[out] tamanhos Vetor de tamanho @p natributos que recebe a quantidade de linhas de cada ramo.
 *
 * @return Vetor com as linhas das diferentes colunas que serão usadas nas divisões.
 */
int** particionar(int** matriz, int* linhas, int sublinhas, int coluna, int natributos, int* tamanhos)
{
    int** ramos = malloc(natributos * sizeof(int*));
    for(int atr_atual = 0; atr_atual < natributos; atr_atual++)
    {
        tamanhos[atr_atual] = 0;
        ramos[atr_atual] = calloc(sublinhas, sizeof(int));
        for(int posicao = 0; posicao < sublinhas; posicao++)
        {
            if(matriz[linhas[posicao]][coluna] == atr_atual)
            {
                ramos[atr_atual][tamanhos[atr_atual]++] = linhas[posicao];
            }
        }
    }

    return ramos;
}

/**
 * @brief Define a classe do atributo desejado com maior representatividade no nó.
 * 
 * Faz a contagem geral usando o vetor @c contagem de cada classe na @p matriz, usando as @p linhas
 * do subconjunto atual.
 *
 * @param[in] matriz Matriz de onde será feita a contagem.
 * @param[in] linhas Índices das linhas do subconjunto do nó atual.
 * @param[in] tamanho Quantidade de linhas do subconjunto.
 * @param[in] num_classes Número de classes desejadas diferentes.
 * @return Índice da classe com maior representatividade no subconjunto atual, ou, -1 se o subconjunto não tiver linhas.
 */
int majoritaria(int** matriz,  int* linhas, int tamanho, int num_classes)
{
    int* contagem = calloc(num_classes, sizeof(int));
    
    int maior = 0;
    int ind_maior = -1;

    for(int i = 0; i < tamanho; i++)
    {
        int classe = matriz[linhas[i]][0];
        contagem[classe]++;   
        if(contagem[classe] > maior)
        {
            maior = contagem[classe];
            ind_maior = classe;
        }
    }

    free(contagem);
    return ind_maior;
}

/**
 * @brief Imprime a árvore com indentação por nível.
 *
 * @param[in] atual Nó a ser impresso.
 * @param[in] trad Tabela usada para recuperar os nomes originais.
 * @param[in] nivel Profundidade da árvore para criar a indentação.
 */
void imprimir_arvore(no* atual, tabela** trad, int nivel)
{
    if(atual->atributo == -1)
    {
        printf("%*s-> %s\n", 4 * nivel, "",trad[0]->refs[atual->classe]);
    }

    else
    {
        for(int i = 0; i < atual->nfilhos; i++)
        {
            printf("%*s[%s = %s]\n", 4 * nivel, "", trad[atual->atributo]->name, trad[atual->atributo]->refs[i]);
            imprimir_arvore(atual->filhos[i], trad, nivel + 1);
        }
    }
}

/**
 * @brief Calculo do Ganho de Informação da coluna escolhida.
 * 
 * IG(S, A) = H(S) - somatório(S_v/S * H(S_v)).
 * H(x) é o calculo da Entropia.
 * É feito uma contagem por pares (valor, classe) que é guardada em vetores, onde o par
 * ocupa a posição [valor * num_classes + classe].
 *
 * @param[in] matriz Tabela traduzida para inteiros.
 * @param[in] trad Tabela de tradução, usado para obter a quantidades de classes e de valores.
 * @param[in] linhas Vetor com os índices das linhas do subconjunto
 * @param[in] coluna Coluna do atributo.
 * @param[in] total Quantidade de linhas em @p linhas.
 * @return Ganho de informação do atributo neste subonconjunto.
 */
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

/**
 * @brief Escolha do melhor ganho de informação entre todos os atributos disponíveis.
 * 
 * Faz o cálculo de Ganho de informação com todos os atributos disponíveis
 * percorrendo todas as colunas da matriz.
 *
 * @param[in] matriz Matriz de inteiros correspondendo a tabela.
 * @param[in] trad Vetor de tabelas
 * @param[in] linhas Índice das linhas do subconjunto.
 * @param[in,out] atributos_usado Atributos que já foram utilizados anteriormente na árvore.
 * @param[in] sublinhas Quantidadde de linhas do subconjunto.
 * @param[in] ncolunas Quantidade total de atributos.
 * @param[out] melhor_ig Registra o melhor ganho de informação.
 * @return Retorna o índice do atributo com o melhor ganho de informação, retorna -1 se não houverem atributos disponíveis.
 */
int melhor_atributo(int** matriz, tabela** trad, int* linhas, int* atributos_usado, int sublinhas, int ncolunas, double* melhor_ig)
{
    int melhor_indice = -1;
    double melhor_ganho = -1.0;

    for(int coluna = 1; coluna < ncolunas; coluna++)
    {
        if(!atributos_usado[coluna])
        {
            double temp = ganho_informacao(matriz, trad, linhas, coluna, sublinhas);
            if(temp > melhor_ganho)
            {
                melhor_ganho = temp;
                melhor_indice = coluna;
            }
        } 
    }

    *melhor_ig = melhor_ganho;
    return melhor_indice;
}

/**
 * @brief Verifica se todos os exemplos do subconjunto tem a mesma classe.
 * 
 * @param[in] matriz Matriz de valores da tabela traduzido para Inteiros.
 * @param[in] linhas Índice das linhas do subconjunto.
 * @param[in] sublinhas Quantidade de linhas do subconjunto.
 * @retval 1 O subconjunto é puro(Ou está vazio).
 * @retval 0 O subconjunto tem pelo menos duas classes diferentes.
 */
int eh_puro(int** matriz, int* linhas, int sublinhas)
{
    int counter;
    for(int i = 0; i < sublinhas; i++)
    {
        int valor = matriz[linhas[i]][0];
        if(i == 0) counter = valor;
        else if(valor != counter) return 0;
    }

    return 1;
}

/**
 * @brief Constrói recursivamente a árvore de decisão.
 * 
 * O nó atual virá folha quando:
 * - o subconjunto é puro.
 * - quando ultrapassa a profundidade máxima.
 * - quando o caminho não tem mais atributos a serem usados.
 *
 * Caso contrário, o nó vai ser dividido pelo atributo com maior ganho.
 * Ramos sem exemlo viram folhas com a classe majoritária do pai.
 * O atributo usado fica apenas marcado na recursão de seus filhos.
 *
 * @param[in] matriz Matriz de valores da tabela traduzidos para inteiros.
 * @param[in] trad Vetor de tabelas.
 * @param[in,out] atributos_usados Vetor que define quais atributos já foram usados
 * para dividir o caminho atual da árvore.
 * @param[in] linhas Índices das linhas do subconjunto atual.
 * @param[in] sublinhas Quantidade de linhas presentes no subconjunto atual.
 * @param[in] ncolunas Quantidade total de colunas.
 * @param[in] profundidade Profundidade do nó atual (raiz tem 0).
 * @return Raiz da subárvore construida.
 */
no* montar_arvore(int** matriz, tabela** trad, int* atributos_usados, int* linhas, int sublinhas, int ncolunas, int profundidade)
{
    int classe = majoritaria(matriz, linhas, sublinhas, trad[0]->nvalores);
    if(eh_puro(matriz, linhas, sublinhas) || profundidade >= MAX_HEIGHT)
    {
        return nova_folha(classe);
    }

    double melhor_ig;
    int melhor_indice = melhor_atributo(matriz, trad, linhas, atributos_usados, sublinhas, ncolunas, &melhor_ig);

    if(melhor_indice == -1)
    {
        return nova_folha(classe);
    }

    int nvalores = trad[melhor_indice]->nvalores;
    int* tamanhos = calloc(nvalores, sizeof(int));
    int** subconjuntos = particionar(matriz, linhas, sublinhas, melhor_indice, nvalores, tamanhos);

    no* atual = novo_no(classe, nvalores, melhor_indice);

    atributos_usados[melhor_indice] = 1;

    for(int valor = 0; valor < nvalores; valor++)
    {
        if(tamanhos[valor] == 0)
        {
            atual->filhos[valor] = nova_folha(classe); 
        }
        else atual->filhos[valor] = montar_arvore(matriz, trad, atributos_usados, subconjuntos[valor], tamanhos[valor], ncolunas, profundidade + 1);
    }
        
    atributos_usados[melhor_indice] = 0;

    for(int i = 0; i < nvalores; i++)
    {
        free(subconjuntos[i]);
    }
    free(subconjuntos);
    free(tamanhos);

    return atual;
}


/**
* @brief Liberação de memória da Árvore de Decisão.
*
* @param[in] raiz Nó da árvore de decisão que será liberada.
*/
void liberar_arvore(no* raiz)
{
    if(raiz == NULL) return;

    for(int i = 0; i < raiz->nfilhos; i++)
    {
        liberar_arvore(raiz->filhos[i]);
    }
    free(raiz->filhos);
    free(raiz);
}


/**
* @brief Liberação de memória da matriz.
*
* @param[in] matriz A matriz que será liberada.
* @param[in] nlinhas Número de linhas da matriz.
*/
void liberar_matriz(int** matriz, int nlinhas)
{
    for(int i = 0; i < nlinhas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}


/**
* @brief Liberação de memória da tabela de tradução.
*
* @param[in] trad Tabela de tradução que será liberada.
* @param[in] ncolunas Número de Atributos da tabela de tradução.
*/
void liberar_trad(tabela** trad, int ncolunas)
{
    for(int i = 0; i < ncolunas; i++)
    {
        for(int j = 0; j < trad[i]->nvalores)
        {
            free(trad[i]->refs[j]);
        }
        free(trad[i]->refs);
        free(trad[i]->name);
        free(trad[i]);
    }

    free(trad);
}

/**
 * @brief Ponto de entrada do programa.
 * 
 * Etapas:
 * -# Leitura do CSV, criando uma tabela de tradução para cada atributo no cabeçalho.
 * -# Leitura de todas as linhas para receber todos os valores possíveis para os atributos.
 * -# Volta ao início do arquivo e preenche a matriz de inteiros usando @c tradutor.
 * -# Constrói a árvore com @c montar_arvore e a imprime com @c imprimir_arvore.
 *
 * @param[in] argc Quantidade de argumentos da linha de comando.
 * @param[in] argv Nome dos argumentos; argv[1] deve ser o nome do .CSV.
 * @retval 0 Execução bem-sucedida.
 * @retval 1 Arquivo não informado ou não pôde ser aberto.
 */
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
            if(tamanho) tamanho += 2;
            else tamanho = 4;
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
    int coluna_atual = 0;

    while(fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';
        for(token = strtok(line, ","); token != NULL; token = strtok(NULL, ","))
        {
            setup(trad, token, coluna_atual);
            coluna_atual++;
        }
        
        coluna_atual = 0;
        nlinhas++;
    }

    rewind(f);    liberar_matriz(matriz, nlinhas);
    liberar_tabelas(trad, ncolunas);
    free(linhas);
    free(atributos_usados);

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
    
    fclose(f);

    int* linhas = malloc(nlinhas * sizeof(int));
    
    for(int i = 0; i < nlinhas; i++)
    {
        linhas[i] = i;
    }

    int* atributos_usados = calloc(ncolunas, sizeof(int));

    no* raiz = montar_arvore(matriz, trad, atributos_usados, linhas, nlinhas, ncolunas, 0);

    imprimir_arvore(raiz, trad, 0);

    for(int i = 0; i < ncolunas; i++)
    {
        free(trad[])

    }

    liberar_arvore(raiz);
    liberar_matriz(matriz, nlinhas);
    liberar_tabelas(trad, ncolunas);
    free(linhas);
    free(atributos_usados);

    return 0;
}