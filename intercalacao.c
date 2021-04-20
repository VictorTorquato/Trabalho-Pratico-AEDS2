#include "intercalacao.h"
#include "funcionarios.h"
#include "arvore_binaria.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void intercalacao_basico(char *nome_arquivo_saida, int num_p, Lista *nome_particoes)
{

    int fim = 0; //variavel que controla fim do procedimento
    FILE *out;   //declara ponteiro para arquivo

    //abre arquivo de saida para escrita
    if ((out = fopen(nome_arquivo_saida, "wb")) == NULL)
    {
        printf("Erro ao abrir arquivo de saida\n");
    }
    else
    {
        //cria vetor de particoes
        TVet v[num_p];

        //abre arquivos das particoes, colocando variavel de arquivo no campo f do vetor
        //e primeiro funcionario do arquivo no campo func do vetor
        for (int i = 0; i < num_p; i++)
        {
            v[i].f = fopen(nome_particoes->nome, "rb");
            v[i].aux_p = 0;
            if (v[i].f != NULL)
            {
                fseek(v[i].f, v[i].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le_funcionario(v[i].f);
                if (f == NULL)
                {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posi??o do vetor
                    v[i].func = funcionario(INT_MAX, "", "", "", 0);
                }
                else
                {
                    //conseguiu ler funcionario, coloca na posi??o atual do vetor
                    v[i].func = f;
                }
            }
            else
            {
                fim = 1;
            }
            nome_particoes = nome_particoes->prox;
        }

        int aux = 0;
        while (!(fim))
        { //conseguiu abrir todos os arquivos
            int menor = INT_MAX;
            int pos_menor;
            //encontra o funcionario com menor chave no vetor
            for (int i = 0; i < num_p; i++)
            {
                if (v[i].func->cod < menor)
                {
                    menor = v[i].func->cod;
                    pos_menor = i;
                }
            }
            if (menor == INT_MAX)
            {
                fim = 1; //terminou processamento
            }
            else
            {
                //salva funcionario no arquivo de saída
                fseek(out, aux * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[pos_menor].func, out);
                //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
                v[pos_menor].aux_p++;
                fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le_funcionario(v[pos_menor].f);
                aux++;
                if (f == NULL)
                {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posiçao do vetor
                    v[pos_menor].func = funcionario(INT_MAX, "", "", "", 0.0);
                }
                else
                {
                    v[pos_menor].func = f;
                }
            }
        }

        //fecha arquivos das partiÇões de entrada
        for (int i = 0; i < num_p; i++)
        {
            fclose(v[i].f);
            //    free(v[i].func);
        }
        //fecha arquivo de saída
        fclose(out);
    }
}

void intercalacao_arvore_de_vencedores(TPilha **pilha, int *vetTop, char *nome_arquivo_saida, int nParticoes, int nFunc)
{
    FILE *out;

    if ((out = fopen(nome_arquivo_saida, "wb"))) //abre arquivo de saida
    {
        printf("Intercalação com arvore de vencedores\n");
    }
    else
    {
        printf("Erro ao abrir arquivo de saida\n");
        return;
    }

    int tamBase = nParticoes;
    TNoA **base = malloc(tamBase * sizeof(TNoA*));
    TFunc **hash = malloc(nFunc * sizeof(TFunc*));

    for (int i = 0; i < nParticoes; i++)    // cria a base da arvore
    {
        TFunc *func = pop(pilha[i], 0, &vetTop[i]);
        hash[func->cod] = func;
        TNoA *no = criaNo_arvore_binaria(func->cod, -1);
        no->pilha = i;
        base[i] = no;
    }

    TNoA *paiPrincipal;
    TNoA **pais = malloc(tamBase * sizeof(TNoA*));
    TNoA **pais2 = malloc(tamBase * sizeof(TNoA*));

    int loop = 0;
    int contPais = 0;
    int contPais2 = 0;
    int funcNoArquivo = 0;

    // gera a primeira versao da arvore de vencedores
    while (1) //primeira iteração
    {
        TNoA **atual;
        int loopTam;
        if(loop == 0)   //caso estiver no primeiro loop
        {
            atual = base;    //atual recebe a base da arvore
            loopTam = tamBase;
        }
        else            //caso não estiver no primeiro loop
        {
            atual = pais;    //atual recebe os pais da arvore
            loopTam = contPais;
        }
        for (int i = 0; i < loopTam; i += 2) //cria os nós na primeira iteração
        {
            if (i + 1 == loopTam)   //se chegar no último registro armazena ele em um novo no
            {
                TNoA *no = criaNo_arvore_binaria(atual[i]->info, -1);
                no->pilha = atual[i]->pilha;
                no->esq = atual[i];
                no->dir = NULL;
                no->esq->pai = no;
                if (loop == 0)
                {
                    pais[contPais] = no;
                    contPais++;
                }
                else
                {
                    pais2[contPais2] = no;
                    contPais2++;
                }
                continue;
            }

            if (atual[i]->info < atual[i + 1]->info) //cria um nó com o menor
            {
                TNoA *no = criaNo_arvore_binaria(atual[i]->info, -1);
                no->pilha = atual[i]->pilha;
                no->esq = atual[i];
                no->dir = atual[i + 1];
                no->esq->pai = no;
                no->dir->pai = no;
                if (loop == 0)
                {
                    pais[contPais] = no;
                    contPais++;
                }
                else
                {
                    pais2[contPais2] = no;
                    contPais2++;
                }
            }
            else
            {
                TNoA *no = criaNo_arvore_binaria(atual[i + 1]->info, -1);
                no->pilha = atual[i + 1]->pilha;
                no->esq = atual[i];
                no->dir = atual[i + 1];
                no->esq->pai = no;
                no->dir->pai = no;
                if (loop == 0)
                {
                    pais[contPais] = no;
                    contPais++;
                }
                else
                {
                    pais2[contPais2] = no;
                    contPais2++;
                }
            }
        }

        // atualiza o vetor de pais
        int aux = loop == 0 ? contPais : contPais2;
        if (aux == 1)
        {
            paiPrincipal = loop == 0 ? pais[0] : pais2[0];
            break;
        }
        if (loop == 0)
        {
            loop++;
            continue;
        }
        for (int j = 0; j < contPais2; j++)
        {
            pais[j] = pais2[j];
        }
        contPais = contPais2;
        contPais2 = 0;
        loop++;
    }

    TNoA *arvore = paiPrincipal;

    while (arvore->info != INT_MAX)
    {
        int vencedor = paiPrincipal->info;
        // DESCE ATÉ A FOLHA DO NÓ VENCEDOR
        while (arvore->esq != NULL || arvore->dir != NULL)
        {
            if (arvore->esq != NULL)
            {
                if (arvore->esq->info == vencedor)
                {
                    arvore = arvore->esq;
                    continue;
                }
            }
            if (arvore->dir != NULL)
            {
                if (arvore->dir->info == vencedor)
                {
                    arvore = arvore->dir;
                    continue;
                }
            }
        }

        // salvar funcionario no arquivo
        TFunc *func = hash[vencedor];
        fseek(out, funcNoArquivo * tamanho_registro(), SEEK_SET);
        funcNoArquivo++;
        salva_funcionario(func, out);
        int stack = paiPrincipal->pilha;

        func = pop(pilha[stack], 0, &vetTop[stack]);
        // printf("Retirou da pilha o %d\n", func->cod);
        if (func == NULL)
            arvore->info = INT_MAX;
        else
        {
            arvore->info = func->cod;
            hash[func->cod] = func;
        }

        // ATUALIZA A ÁRVORE DE VENCEDORES COM O NOVO VALOR E JÁ O COMPARA COM OS VALORES ALI JÁ PRESENTES
        while (arvore->pai != NULL)
        {
            arvore = arvore->pai;
            int prevValue = arvore->info;
            if (arvore->esq && arvore->dir)
            {
                if (arvore->esq->info < arvore->dir->info)
                {
                    arvore->info = arvore->esq->info;
                    arvore->pilha = arvore->esq->pilha;
                }
                else
                {
                    arvore->info = arvore->dir->info;
                    arvore->pilha = arvore->dir->pilha;
                }
            }
            else
            {
                if (arvore->esq != NULL)
                {
                    arvore->info = arvore->esq->info;
                    arvore->pilha = arvore->esq->pilha;
                }
                else
                {
                    arvore->info = arvore->dir->info;
                    arvore->pilha = arvore->dir->pilha;
                }
            }

        }
    }
    free(base);
    free(hash);
    free(pais);
    free(pais2);
}
