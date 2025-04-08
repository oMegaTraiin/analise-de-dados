#include <stdio.h>
#include <stdlib.h>

#include "func.h"

int main() {
	int size, id, count;
    Processo *dados = LerDados("processo_043_202409032338.csv", &size);
    printf("Foram lidos %d processos\n", size);

    // Organizar por id e salvar arquivo csv;
    qSortById(dados, 0, size - 1);
    SalvarDados("processos-order-by-id.csv", dados, size);

    // Organizar por data e salvar arquivo csv;
    qSortByData(dados, 0, size - 1);
    SalvarDados("processos-order-by-data.csv", dados, size);

    // Contar quantos processos tem vinculados a um determinado "id_classe";
    id = 12554;
    count = countProcessosLinkedIdClasse(dados, size, id);
    if (count == 0)
        printf("Nenhum processo foi encontrado vinculado com esse ID\n");
    else
        printf("%d processo(s) estão vinculados ao 'id_classe' %d\n", count, id);

    // Contar quantos "id_assuntos" constam na base de dados;
    count = countIdAssuntos(dados, size);
    printf("%d 'id_assuntos' constam nos processos presentes na base de dados.\n", count);

    // Listar todos os processos com mais de um "id_assuntos";
    listProcessosHaveManyAssuntos(dados, size);

    // Calcular dias de tramitação de um processo;
    id = 638633058;
    calcTramitacaoByID(dados, size, id);

    free(dados);
    return 0;
}