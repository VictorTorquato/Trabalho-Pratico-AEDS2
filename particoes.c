
#include "funcionarios.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#include "particoes.h"

// Fun��o que le um funcionario do arquivo e o retorna
// tamb�m incrementa o total de funcionarios lidos
TFunc* getFunc(FILE* arq, int* contLidos)
{
    fseek(arq, *contLidos * tamanho_registro(), SEEK_SET);
    TFunc* func = le_funcionario(arq);
    (*contLidos)++;
    return func;
}

// Fun��o que recebe o indice do menor elemento do vetor,
// joga ele para o inicio e retorna sua posi��o
int menorParaInicio(TFunc* v[], int M, int menor)
{
    TFunc* aux;
    if(menor == 0)
    {
        return(0);
    }
    else
    {
        for(int i = M-1; i > 0; i--)
        {
            if (i == menor)
            {
                aux = v[menor];
                v[i] = v[i-1];
                v[i-1] = aux;
                menor--;
            }
        }
    }
    return(menor);
}

// Fun��o que retorna a quantidade de elementos dentro do vetor
int elementosNoVetor(bool controle[], int tam)
{
    int aux = 0;
    for (int i = 0; i < tam; i++)
    {
        if (controle[i] == false)
        {
            aux++;
        }
    }
    return(aux);
}

// Procedimento que cria novas parti��es caso necess�rio
void atualizaNomesParticao(Lista* nomes, int* nParticoes)
{
    if(nomes->prox == NULL)
    {
        char* newNome = malloc(5 * sizeof(char));
        (*nParticoes)++;
        sprintf(newNome, "p%d.dat", *nParticoes);
        nomes->prox = cria(newNome, NULL);
    }
}

// Fun��o que retorna o �ndice do menor registro do vetor
int getMenor(TFunc* v[], int tam)
{
    int menor = 0;
    for (int i = 1; i < tam; i++)
    {
        if(v[menor]->cod > v[i]->cod)
        {
            menor = i;
        }
    }
    return menor;
}

// Fun��o para verificar se um vetor de tamanho tam possui elementos
int possuiElementos(TFunc* v[], int tam)
{
    for (int i = 0; i < tam; i++)
    {
        if(v[i] != NULL){
            return(1);
        }
    }
    return(0);
}

void classificacao_interna(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc)
{
    rewind(arq); //posiciona cursor no inicio do arquivo
    int reg = 0;

    while (reg != nFunc)
    {
        //le o arquivo e coloca no vetor
        TFunc *v[M];
        int i = 0;
        while (!feof(arq))
        {
            fseek(arq, (reg) * tamanho_registro(), SEEK_SET);
            v[i] = le_funcionario(arq);
            //     imprime_funcionario(v[i]);
            i++;
            reg++;
            if(i>=M)
                break;
        }

        //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
        if (i != M)
        {
            M = i;
        }

        //faz ordenacao
        for (int j = 1; j < M; j++)
        {
            TFunc *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->cod > f->cod))
            {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao
        char *nome_particao = nome_arquivos_saida->nome;
        nome_arquivos_saida = nome_arquivos_saida->prox;
        printf("\n%s\n", nome_particao);
        FILE *p;
        if ((p = fopen(nome_particao, "wb+")) == NULL)
        {
            printf("Erro criar arquivo de saida\n");
        }
        else
        {
            for (int i = 0; i < M; i++)
            {
                fseek(p, (i) * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[i], p);
                imprime_funcionario(v[i]);
            }
            fclose(p);
        }
        for(int jj = 0; jj<M; jj++)
            free(v[jj]);
    }
}

void selecao_natural(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc, int n, int* nParticoes)
{
    Lista* nomes = nome_arquivos_saida;
    Lista* nome_part = nomes;
    FILE* particao = NULL;
    int cursorPart = 0;
    int totalLidos = 0;

    TFunc** v = malloc(M * sizeof(TFunc*));

    FILE* res = fopen("reservatorio.dat", "w+");
    int cursorRes = 0;
    bool reservatorio_cheio;

    bool controleVet[M];
    int i = 0;

    // M registros do arquivo para a memoria
    while (i < M && (!feof(arq)))
    {
        v[i] = getFunc(arq, &totalLidos);
        controleVet[i] = false;
        i++;
    }

    // Abre arquivo de parti��o
    if(particao == NULL)
    {
        char* nome = nomes->nome;
        particao = fopen(nome, "wb+");
        nome_part = nomes;

        atualizaNomesParticao(nomes, nParticoes);
        nomes = nomes->prox;
    }

    // Enquanto h� elementos no vetor e
    // todos os registros n�o foram lidos
    while (possuiElementos(v, M) && totalLidos <= nFunc)
    {
        reservatorio_cheio = false;
        // Enquanto n�o chega ao fim do arquivo e
        // o reservatorio n�o est� cheio
        while (!feof(arq) && !reservatorio_cheio)
        {
            // Caso n�o haja parti��o aberta
            // abre a pr�xima parti��o
            if(particao == NULL)
            {
                char* nome = nomes->nome;
                particao = fopen(nome, "wb+");
                nome_part = nomes;

                atualizaNomesParticao(nomes, nParticoes);
                nomes = nomes->prox;
            }
            // Caso a parti��o atual esteja  cheia
            if(cursorPart >= M)
            {
                fclose(particao);
                particao = NULL;
                cursorPart = 0;
                continue;
            }
            // Pega o menor valor do vetor
            int menor = getMenor(v, M);
            menor = menorParaInicio(v, M, menor);

            // Salva o menor registro na parti��o
            fseek(particao, cursorPart * tamanho_registro(), SEEK_SET);
            salva_funcionario(v[menor], particao);

            // Salva o cod do registro particionado acima
            int lastKey = v[menor]->cod;
            cursorPart++;
            nome_part->tamanho++;
            controleVet[menor] = true;

            // Enquanto o indice do registro particionado n�o for trocado e
            // nenhuma das condi��es de parada anteriores for verdadeira
            while (controleVet[menor] && !reservatorio_cheio && !feof(arq) && totalLidos < nFunc)
            {
                if (!feof(arq))
                {
                    // Troca o registro j� particionado com pr�ximo registro do arquivo
                    v[menor] = getFunc(arq, &totalLidos);

                    // Se for maior do que o registro da parti��o
                    // grava na parti��o normalmente
                    if (v[menor]->cod >= lastKey)
                    {
                        // Caso a parti��o atual esteja cheia
                        // abre a pr�xima parti��o
                        if(cursorPart >= M)
                        {
                            fclose(particao);
                            particao = NULL;
                            cursorPart = 0;

                            char* nome = nomes->nome;
                            particao = fopen(nome, "wb+");
                            nome_part = nomes;

                            atualizaNomesParticao(nomes, nParticoes);
                            nomes = nomes->prox;
                        }
                        fseek(particao, cursorPart * tamanho_registro(), SEEK_SET);
                        salva_funcionario(v[menor], particao);
                        controleVet[menor] = false;
                    }
                    // Se for menor do que o registro da parti��o
                    // grava no reservat�rio e substitui pelo pr�ximo
                    else
                    {
                        fseek(res, cursorRes * tamanho_registro(), SEEK_SET);
                        salva_funcionario(v[menor], res);
                        cursorRes++;
                        if (cursorRes == n)
                        {
                            reservatorio_cheio = true;
                        }
                    }
                }
            }
        }
        // Se ja leu todos os registros est� no fim da execu��o
        // Abre nova parti��o e armazena os registros restantes
        if(totalLidos == nFunc)
        {
            fclose(particao);
            particao = NULL;
            cursorPart = 0;
            char* nome = nomes->nome;
            particao = fopen(nome, "wb+");
            nome_part = nomes;

            for(i = 0; i < M; i++)
            {
                fseek(particao, i * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[i], particao);
            }
            cursorPart = 0;
            cursorRes = 0;
        }
        // Se a parti��o est� cheia abre a pr�xima
        if(cursorPart >= M)
        {
            fclose(particao);
            particao = NULL;
            cursorPart = 0;

            char* nome = nomes->nome;
            particao = fopen(nome, "wb+");
            nome_part = nomes;

            atualizaNomesParticao(nomes, nParticoes);
            nomes = nomes->prox;
        }
        // Se h� elementos no reservat�rio, os passa para o vetor
        if (cursorRes > 0)
        {
            for (i = 0; i < cursorRes; i++)
            {
                fseek(res, i * tamanho_registro(), SEEK_SET);
                v[i] = le_funcionario(res);
                controleVet[i] = false;
                cursorRes--;
            }
        }
        int cont = elementosNoVetor(controleVet, M);

        // Se h� espa�o no vetor, o preenche com registros do arquivo
        if (cont != M && (!feof(arq)))
        {
            while (cont < M && (!feof(arq)))
            {
                v[cont] = getFunc(arq, &totalLidos);
                controleVet[cont] = false;
                cont++;
            }
        }
    }
    // Libera a memoria alocada
    for (int i = 0; i < M; i++)
    {
        free(v[i]);
    }
    fclose(particao);
    free(v);
}
