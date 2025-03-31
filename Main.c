#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

// Funções de ordenação;
void swap(Processo *a, Processo *b);
int partionById(Processo *V, int inf, int sup);
int partionByData(Processo *V, int inf, int sup);
void qSortById(Processo *V, int inf, int sup);
void qSortByData(Processo *V, int inf, int sup);
int compareDates(Data d1, Data d2);

// Outros
int countProcessosLinkedIdClasse(Processo *V, int size, int id_classe);
int countIdAssuntos(Processo *V, int size);
void listProcessosHaveManyAssuntos(Processo *V, int size);

// Datas;
long calcular_diferenca_segundos(Data data_inicio, Data data_fim);
void calcular_tramitacao_por_id(Processo *processos, int size, int id_processo);

int main() {
	int size;
    Processo *dados = LerDados("processos.csv", &size);

    printf("Foram lidos %d processos\n", size);
    //calcular_tramitacao_por_id(dados, size, 405286372);

    qSortById(dados, 0, size - 1);
    SalvarDados("processos-order-by-id.csv", dados, size);

    qSortByData(dados, 0, size - 1);
    SalvarDados("processos-order-by-data.csv", dados, size);

    //listProcessosHaveManyAssuntos(dados, size);

    //int id_classe = 12554, count = countProcessosLinkedIdClasse(dados, size, id_classe);

    //printf("%d Processos estao vinculados com o ID Classe: %d", count, id_classe);

    /*
    for (int i = 0; i < 20; i++) {
        printf("Linha: %d\nID: %d\nNumero: %s", i + 2, dados[i].id, dados[i].numero);

        printf("\nData: %d-%d-%d %d:%d:%d.000", dados[i].data_ajuizamento.ano, dados[i].data_ajuizamento.mes, dados[i].data_ajuizamento.dia, dados[i].data_ajuizamento.hora, dados[i].data_ajuizamento.min, dados[i].data_ajuizamento.sec);

        printf("\nID Classes: ");
        for (int j = 0; j < dados[i].qtd_classes; j++) {
            printf("%d ", dados[i].id_classes[j]);
        }

        printf("\nID Assuntos: ");
        for (int j = 0; j < dados[i].qtd_assuntos; j++) {
            printf("%d ", dados[i].id_assuntos[j]);
        }
        printf("\nAno Eleicao: %d", dados[i].ano_eleicao);
        printf("\n\n");
    }
    */

    free(dados);
    return 0;
}

Processo *LerDados(const char *NomeArquivo, int *quantidade) {
    FILE *fp = fopen(NomeArquivo, "r");
    if (!fp) {
        printf("Erro ao abrir o arquivo!\n");
        return 0;
    }

    int capacidade = 20;
	Processo *X = malloc(capacidade * sizeof(Processo));
	if (!X) {
        printf("Erro ao alocar memória!\n");
        fclose(fp);
        return NULL;
    }

    char linha[100];
    fgets(linha, sizeof(linha), fp);
	
    int i = 0;
    while (fgets(linha, sizeof(linha), fp)) {
        // Realoca memoria caso seja necessario;
        if (i >= capacidade) {
            capacidade += 20;
            X = realloc(X, capacidade * sizeof(Processo));
            if (!X) {
                printf("ERRO: Falha ao realocar memoria!\n");
                exit(1);
            }
        }

        char id_classes[25], id_assuntos[25], data[25];

        //printf("\nLinha: %d", i + 1);
        // Ler ID;
        char *token = strtok(linha, ",");
        if (token) {
            //printf("\nID: %s", token);
            X[i].id = atoi(token);
        }
 
        // Ler Numero;
        token = strtok(NULL, ",");
        if (token) {
            if (token[0] == '\"') {
                token++;  // Remove a primeira aspas;
                token[strlen(token) - 1] = '\0';  // Remove a última aspas;
            }
            //printf("\nNumero: %s", token);
            strncpy(X[i].numero, token, 25);
        }
 
        // Ler data como string;
        token = strtok(NULL, ",");
        if (token) {
            //printf("\nData: %s",token);
            strncpy(data, token, 25);
        }

        // Ler id_classe (suportando multiplos valores);
        token = strtok(NULL, "}");
        if (token) {
            while (token[0] == '\"' || token[0] == '{') {
                token++;  // Remove a primeira aspas
            }
            //printf("\nClasses: %s",token);
            strncpy(id_classes, token, 25);
        }

        // Ler id_assunto (suportando multiplos valores);
        token = strtok(NULL, "}");
        if (token) {
            while (token[0] == ',' || token[0] == '\"' || token[0] == '{') {
                token++;  // Remove a primeira aspas
            }
            //printf("\nAssuntos: %s",token);
            strncpy(id_assuntos, token, 25);
        }

        token = strtok(NULL, ",");
        if (token) {
            //printf("\nAno: %s",token);
            X[i].ano_eleicao = token ? atoi(token) : 0;
        }

        extrairMultivalorados(id_classes, X[i].id_classes, &X[i].qtd_classes);
        extrairMultivalorados(id_assuntos, X[i].id_assuntos, &X[i].qtd_assuntos);
        extrairData(data, &X[i].data_ajuizamento);

        i++;
    }

	*quantidade = i;
    fclose(fp);
	return X;
}

void SalvarDados(const char *nome_arquivo, Processo *V, int size) {
    FILE *arquivo = fopen(nome_arquivo, "w");  // Abre o arquivo em modo de escrita

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    // Escreve o cabeçalho do CSV
    fprintf(arquivo, "id;numero;data_ajuizamento;id_classe;id_assunto;ano_eleicao\n");

    // Escreve os dados de cada processo no arquivo CSV
    for (int i = 0; i < size; i++) {
        // Escreve os dados de cada processo separados por vírgulas
        fprintf(arquivo, "%d,\"%s\",%d-%d-%d %02d:%02d:%02d.000,", 
                V[i].id, V[i].numero,
                V[i].data_ajuizamento.ano,
                V[i].data_ajuizamento.mes,
                V[i].data_ajuizamento.dia,
                V[i].data_ajuizamento.hora,
                V[i].data_ajuizamento.min,
                V[i].data_ajuizamento.sec);
        
        // Escreve os ids das classes no arquivo

        fprintf(arquivo, V[i].qtd_classes > 1 ? "\"{" : "{");
        for (int j = 0; j < V[i].qtd_classes; j++) {
            fprintf(arquivo, "%d", V[i].id_classes[j]);
            if (j < V[i].qtd_classes - 1) {
                fprintf(arquivo, ",");  // Se não for o último, separa por espaço
            }
        }
        fprintf(arquivo, V[i].qtd_classes > 1 ? "}\"," : "},");

        // Escreve os ids dos assuntos no arquivo
        fprintf(arquivo, V[i].qtd_assuntos > 1 ? "\"{" : "{");
        for (int j = 0; j < V[i].qtd_assuntos; j++) {
            fprintf(arquivo, "%d", V[i].id_assuntos[j]);
            if (j < V[i].qtd_assuntos - 1) {
                fprintf(arquivo, " ");  // Se não for o último, separa por espaço
            }
        }
        fprintf(arquivo, V[i].qtd_assuntos > 1 ? "}\"," : "},");

        // Escreve a quantidade de assuntos e o ano da eleição
        fprintf(arquivo, "%d\n", V[i].ano_eleicao);
    }

    fclose(arquivo);  // Fecha o arquivo
    printf("Dados salvos com sucesso no arquivo %s.\n", nome_arquivo);
}

void extrairMultivalorados(char *str, int *values, int *count) {
    *count = 0;
    char *token = strtok(str, ",");
    while (token != NULL) {
        values[(*count)++] = atoi(token);
        token = strtok(NULL, ",");
    }
}

void extrairData(char *str, Data *data) {
    int ano, mes, dia, hora, min, sec;
    if (sscanf(str, "%d-%d-%d %d:%d:%d.000", &ano, &mes, &dia, &hora, &min, &sec) == 6){
        data->ano = ano; 
        data->mes = mes;
        data->dia = dia;
        data->hora = hora;
        data->min = min;
        data->sec = sec;
    } else {
        printf("Formato de data/hora inválido.\n");
    }
}

void swap(Processo *a, Processo *b) {
    Processo temp = *a;
    *a = *b;
    *b = temp;
}

// Função para comparar duas datas
int compareDates(Data d1, Data d2) {
    if (d1.ano != d2.ano) {
        return d1.ano - d2.ano;  // Retorna a diferença entre os anos
    }
    if (d1.mes != d2.mes) {
        return d1.mes - d2.mes;  // Retorna a diferença entre os meses
    }
    if (d1.dia != d2.dia) {
        return d1.dia - d2.dia;  // Retorna a diferença entre os dias
    }
    if (d1.hora != d2.hora) {
        return d1.hora - d2.hora;  // Retorna a diferença entre as horas
    }
    if (d1.min != d2.min) {
        return d1.min - d2.min;  // Retorna a diferença entre os minutos
    }
    return d1.sec - d2.sec;  // Retorna a diferença entre os segundos
}

int partionByData(Processo *V, int inf, int sup) {
    Data Pivot = V[(inf + sup) / 2].data_ajuizamento;
    int i = inf;
    int j = sup;

    while (i <= j) {
        while (compareDates(V[i].data_ajuizamento, Pivot) > 0) i++;  // Data mais antiga
        while (compareDates(V[j].data_ajuizamento, Pivot) < 0) j--;  // Data mais recente
        
        if (i <= j) {
            swap(&V[i], &V[j]);  // Troca os processos inteiros
            i++;
            j--;
        }
    }
    return i;  // Retorna a posição para dividir o array
}

int partionById(Processo *V, int inf, int sup) {
    int Pivot = V[(inf + sup) / 2].id;  // Usa o campo 'id' como pivô
    int i = inf;
    int j = sup;

    while(i <= j) {
        while (V[i].id < Pivot) i++;  // Compara pelo campo 'id'
        while (V[j].id > Pivot) j--;  // Compara pelo campo 'id'
        
        if(i <= j) {
            swap(&V[i], &V[j]);  // Troca os processos inteiros
            i++;
            j--;
        }
    }
    return i;  // Retorna a posição para dividir o array
}

void qSortById(Processo *V, int inf, int sup) {
	if (inf < sup) {
		int p = partionById(V, inf, sup);
		qSortById(V, inf, p - 1);
		qSortById(V, p, sup);
	}
}

void qSortByData(Processo *V, int inf, int sup) {
	if (inf < sup) {
		int p = partionByData(V, inf, sup);
		qSortByData(V, inf, p - 1);
		qSortByData(V, p, sup);
	}
}

// Função de contar quantos processos estão vinculados a um determinado id_classe;
int countProcessosLinkedIdClasse(Processo *V, int size, int id_classe) {
    int contador = 0;
    // Itera sobre todos os processos
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < V[i].qtd_classes; j++) {
            if (V[i].id_classes[j] == id_classe) {
                contador++;
                break;
            }
        }
    }
    return contador;
}

// Função de identificar quantos id_assuntos constam nos processos presentes na base de dados;
int countIdAssuntos(Processo *V, int size) {
    int contador = 0;
    int N[1000];
    // Itera sobre todos os processos;
    for (int i = 0; size; i++) {
        for (int j = 0; j < V[i].qtd_assuntos; j++) {
            int encontrado = 0;
            for (int k = 0; k < contador; k++) {
                if (N[k] == V[i].id_assuntos[j]) {
                    encontrado = 1;
                    break;
                }
            }

            // Se o id_assunto não foi encontrado, adiciona ele ao vetor
            if (!encontrado) {
                N[contador] = V[i].id_assuntos[j];
                contador++;
            }
        }
    }
    
    return contador;
}

// Função para listar todos os processos que estão vinculados a mais de um assunto
void listProcessosHaveManyAssuntos(Processo *V, int size) {
    printf("Processos vinculados a mais de um assunto:\n");
    // Itera sobre todos os processos;
    for (int i = 0; size; i++) {
        if (V[i].qtd_assuntos > 1) {
            printf("Processo ID: %d, Numero: %s, Quantidade de Assuntos: %d\n", V[i].id, V[i].numero, V[i].qtd_assuntos);
        }
    }
}

// Função para calcular a diferença de segundos entre duas datas (completa: ano, mes, dia, hora, min, sec)
long calcular_diferenca_segundos(Data data_inicio, Data data_fim) {
    struct tm inicio = {0};
    struct tm fim = {0};

    // Preenche a struct tm com a data de início
    inicio.tm_year = data_inicio.ano - 1900;  // tm_year é o ano desde 1900
    inicio.tm_mon = data_inicio.mes - 1;      // tm_mon é de 0 a 11
    inicio.tm_mday = data_inicio.dia;
    inicio.tm_hour = data_inicio.hora;
    inicio.tm_min = data_inicio.min;
    inicio.tm_sec = data_inicio.sec;

    // Preenche a struct tm com a data de fim
    fim.tm_year = data_fim.ano - 1900;
    fim.tm_mon = data_fim.mes - 1;
    fim.tm_mday = data_fim.dia;
    fim.tm_hour = data_fim.hora;
    fim.tm_min = data_fim.min;
    fim.tm_sec = data_fim.sec;

    // Converte para o timestamp (segundos desde 1970)
    time_t inicio_seconds = mktime(&inicio);
    time_t fim_seconds = mktime(&fim);

    // Calcula a diferença em segundos
    return difftime(fim_seconds, inicio_seconds);
}

// Função para calcular a quantidade de dias de tramitação de um processo específico
void calcular_tramitacao_por_id(Processo *processos, int size, int id_processo) {
    // Obtém a data atual
    time_t t = time(NULL);
    struct tm data_atual = *localtime(&t);

    // Exibe a data atual (opcional)
    printf("Data atual: %02d/%02d/%04d %02d:%02d:%02d\n", 
            data_atual.tm_mday, data_atual.tm_mon + 1, data_atual.tm_year + 1900, 
            data_atual.tm_hour, data_atual.tm_min, data_atual.tm_sec);

    // Procura pelo processo com o ID fornecido
    int encontrado = 0;
    for (int i = 0; size; i++) {
        if (processos[i].id == id_processo) {
            // Calcula a diferença em segundos entre a data de ajuizamento e a data atual
            long diferenca_segundos = calcular_diferenca_segundos(processos[i].data_ajuizamento, 
                (Data){data_atual.tm_year + 1900, data_atual.tm_mon + 1, data_atual.tm_mday, 
                       data_atual.tm_hour, data_atual.tm_min, data_atual.tm_sec});

            // Converte a diferença de segundos para dias, anos, meses, horas, minutos e segundos
            int dias_totais = diferenca_segundos / (60 * 60 * 24);
            int anos = dias_totais / 365;  // Considera 365 dias por ano
            int meses = (dias_totais % 365) / 30;  // Aproximação: 30 dias por mês
            int dias = (dias_totais % 365) % 30;
            int horas = (diferenca_segundos % (60 * 60 * 24)) / (60 * 60);
            int minutos = (diferenca_segundos % (60 * 60)) / 60;
            int segundos = diferenca_segundos % 60;

            // Exibe a quantidade de tempo de tramitação
            printf("Processo ID: %d, Numero: %s\n", processos[i].id, processos[i].numero);
            printf("Tempo de tramitação: %d anos, %d meses, %d dias, %d horas, %d minutos, %d segundos\n\n", 
                    anos, meses, dias, horas, minutos, segundos);

            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Processo com ID %d não encontrado.\n", id_processo);
    }
}