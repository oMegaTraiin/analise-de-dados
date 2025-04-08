#include <stdio.h>
#include <stdlib.h>

#include "func.h"

int main() {
	int size;
    Processo *dados = LerDados("processo_043_202409032338.csv", &size);

    printf("Foram lidos %d processos\n", size);

    int res;
    do {
        printf("\nEscolha uma opção:");
        printf("\n1 - Ordenar em ordem crescente pelo 'id'");
        printf("\n2 - Ordenar em ordem decrescente pelo 'data_ajuizamento'");
        printf("\n3 - Contar processos vinculados a um determinado 'id_classe'");
        printf("\n4 - Contar quantos 'id_assuntos' existem na base de dados");
        printf("\n5 - Listar processos com mais de um 'id_assunto'");
        printf("\n6 - Calcular dias de tramitação de um processo");
        printf("\n0 - Sair");
        printf("\n> ");
        scanf("%d", &res);

        switch(res) {
            case 1: {
                qSortById(dados, 0, size - 1);
                printf("Dados organizado por 'id' com sucesso\n");
                SalvarDados("processos-order-by-id.csv", dados, size);
                break;
            }
            case 2: {
                qSortByData(dados, 0, size - 1);
                printf("Dados organizado por 'data_ajuizamento' com sucesso\n");
                SalvarDados("processos-order-by-data.csv", dados, size);
                break;
            }
            case 3: {
                int id = 0;
                do {
                    printf("\nDigite o ID da Classe que deseja verificar: ");
                    printf("\nUtilize '0' para voltar;");
                    printf("\n> ");
                    scanf("%d", &id);
                    if (id == 0) break;

                    int count = countProcessosLinkedIdClasse(dados, size, id);
                    if (count == 0) {
                        printf("Nenhum processo foi encontrado vinculado com esse ID\n");
                    } else {
                        printf("%d processo(s) estão vinculados ao 'id_classe' %d\n", count, id);
                    }
                } while(id != 0);
                break;
            }
            case 4: {
                int count = countIdAssuntos(dados, size);
                printf("%d 'id_assuntos' constam nos processos presentes na base de dados.\n", count);
                break;
            }
            case 5:
                listProcessosHaveManyAssuntos(dados, size);
                break;
            case 6: {
                int id;
                do {
                    printf("\nDigite o ID do Processo que deseja verificar: ");
                    printf("\nUtilize '0' para voltar;");
                    printf("\n> ");
                    scanf("%d", &id);
                    if(id == 0) break;

                    calcTramitacaoByID(dados, size, id);
                } while(id != 0);
                break;
            }
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
        printf("\nPressione ENTER para continuar...");
        while (getchar() != '\n');
        getchar();
    } while(res != 0);

    free(dados);
    return 0;
}