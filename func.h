#ifndef FUNC_H
#define FUNC_H

typedef struct {
    int ano, mes, dia;
    int hora, min, sec;
} Data;

typedef struct {
    int id;
    char numero[25];
    Data data_ajuizamento; 
    int id_classes[10];
    int qtd_classes;
    int id_assuntos[10];
    int qtd_assuntos;
    int ano_eleicao;
} Processo;

Processo *LerDados(const char *NomeArquivo, int *quantidade);
void SalvarDados(const char *nome_arquivo, Processo *V, int size);
void extrairMultivalorados(char *str, int *values, int *count);
void extrairData(char *str, Data *data);

void swap(Processo *a, Processo *b);
int partionById(Processo *V, int inf, int sup);
int partionByData(Processo *V, int inf, int sup);
void qSortById(Processo *V, int inf, int sup);
void qSortByData(Processo *V, int inf, int sup);
int compareDates(Data d1, Data d2);

int countProcessosLinkedIdClasse(Processo *V, int size, int id_classe);
int countIdAssuntos(Processo *V, int size);
void listProcessosHaveManyAssuntos(Processo *V, int size);

long calcDifSecs(Data data_inicio, Data data_fim);
void calcTramitacaoByID(Processo *processos, int size, int id_processo);

#endif