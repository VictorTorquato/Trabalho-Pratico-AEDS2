#ifndef PARTICOES_H
#define PARTICOES_H

#include <stdio.h>
#include <stdbool.h>

#include "lista.h"

/* Algoritmo de geracao de particoes por Classificacao Interna, recebe como parâmetro
o arquivo de dados de entrada, a lista contendo os nomes dos arquivos de saída das partições,
o númer de elementos M a ser armazenado em cada partição e o número de registro total do arquivo.*/
void classificacao_interna(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc);

/*Algoritmo a ser implementado no trabalho de geracao de particoes por Selecao Natural recebe como parâmetro
o arquivo de dados de entrada, a lista contendo os nomes dos arquivos de saída das partições,
o númer de elementos M a ser armazenado em cada partição e o número de registro total do arquivo e o tamanho do reservatório.
Fiquem a vontade para modificar a estrutura, foi uma sugestão.*/
void selecao_natural(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc, int n, int* nParticoes);

/*Função que le um funcionario do arquivo e o retorna
  também incrementa o total de funcionarios lidos*/
TFunc* getFunc(FILE* arq, int* contLidos);

/*Função que recebe o indice do menor elemento do vetor,
  joga ele para o inicio e retorna sua posição*/
int menorParaInicio(TFunc* v[], int M, int menor);

/*Função que retorna a quantidade de elementos dentro do vetor*/
int elementosNoVetor(bool controle[], int tam);

/*Procedimento que cria novas partições caso necessário*/
void atualizaNomesParticao(Lista* nomes, int* nParticoes);

/*Função que retorna o índice do menor registro do vetor*/
int getMenor(TFunc* v[], int tam);

/*Função para verificar se um vetor de tamanho tam possui elementos*/
int possuiElementos(TFunc* v[], int tam);

#endif
