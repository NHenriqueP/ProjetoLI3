#include "app_runner.h"
#include "gestorglobal.h"
#include "querie1.h"
#include "querie2.h"
#include "querie3.h"
#include "querie4.h"
#include "querie5.h"
#include "querie6.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void limpar_newline(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

static void executar_query_interativa(SistemaGestor *gestor, const char *query_num) {
    char linha[512];
    int nout = 1;

    if (strcmp(query_num, "1") == 0) {
        printf("Código do aeroporto: ");
        fflush(stdout);
        if (!fgets(linha, sizeof(linha), stdin)) return;
        limpar_newline(linha);
        
        char query_linha[256];
        snprintf(query_linha, sizeof(query_linha), "1 %s", linha);
        execQ1(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else if (strcmp(query_num, "2") == 0) {
        printf("N (top N aeronaves): ");
        fflush(stdout);
        if (!fgets(linha, sizeof(linha), stdin)) return;
        limpar_newline(linha);
        
        char fabricante[256] = "";
        printf("Fabricante (Enter para todos): ");
        fflush(stdout);
        if (fgets(fabricante, sizeof(fabricante), stdin)) {
            limpar_newline(fabricante);
        }
        
        char query_linha[512];
        if (fabricante[0] != '\0') {
            snprintf(query_linha, sizeof(query_linha), "2 %s %s", linha, fabricante);
        } else {
            snprintf(query_linha, sizeof(query_linha), "2 %s", linha);
        }
        execQ2(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else if (strcmp(query_num, "3") == 0) {
        printf("Data início (YYYY-MM-DD): ");
        fflush(stdout);
        char data_ini[32];
        if (!fgets(data_ini, sizeof(data_ini), stdin)) return;
        limpar_newline(data_ini);
        
        printf("Data fim (YYYY-MM-DD): ");
        fflush(stdout);
        char data_fim[32];
        if (!fgets(data_fim, sizeof(data_fim), stdin)) return;
        limpar_newline(data_fim);
        
        char query_linha[256];
        snprintf(query_linha, sizeof(query_linha), "3 %s %s", data_ini, data_fim);
        execQ3(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else if (strcmp(query_num, "4") == 0) {
        printf("Data início (YYYY-MM-DD, Enter para todas): ");
        fflush(stdout);
        char data_ini[32] = "";
        if (fgets(data_ini, sizeof(data_ini), stdin)) {
            limpar_newline(data_ini);
        }
        
        char query_linha[256];
        if (data_ini[0] != '\0') {
            printf("Data fim (YYYY-MM-DD): ");
            fflush(stdout);
            char data_fim[32];
            if (!fgets(data_fim, sizeof(data_fim), stdin)) return;
            limpar_newline(data_fim);
            snprintf(query_linha, sizeof(query_linha), "4 %s %s", data_ini, data_fim);
        } else {
            snprintf(query_linha, sizeof(query_linha), "4");
        }
        execQ4(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else if (strcmp(query_num, "5") == 0) {
        printf("N (top N companhias): ");
        fflush(stdout);
        if (!fgets(linha, sizeof(linha), stdin)) return;
        limpar_newline(linha);
        
        char query_linha[256];
        snprintf(query_linha, sizeof(query_linha), "5 %s", linha);
        execQ5(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else if (strcmp(query_num, "6") == 0) {
        printf("Nacionalidade: ");
        fflush(stdout);
        if (!fgets(linha, sizeof(linha), stdin)) return;
        limpar_newline(linha);
        
        char query_linha[256];
        snprintf(query_linha, sizeof(query_linha), "6 %s", linha);
        execQ6(query_linha, gestor, &nout);
        
        FILE *f = fopen("resultados/command1_output.txt", "r");
        if (f) {
            char buf[1024];
            printf("\nResultado:\n");
            while (fgets(buf, sizeof(buf), f)) {
                printf("%s", buf);
            }
            fclose(f);
        }
    }
    else {
        printf("Query inválida!\n");
    }
}

int main(void) {
    char pasta_dataset[512];
    
    printf("Introduza o caminho dos ficheiros de dados: ");
    fflush(stdout);
    
    if (!fgets(pasta_dataset, sizeof(pasta_dataset), stdin)) {
        fprintf(stderr, "Erro ao ler caminho.\n");
        return 1;
    }
    limpar_newline(pasta_dataset);
    
    SistemaGestor *gestor = criar_gestor_global();
    if (!gestor) {
        fprintf(stderr, "Erro ao criar gestor.\n");
        return 1;
    }
    
    if (!carregar_dados(pasta_dataset, gestor)) {
        fprintf(stderr, "Erro ao carregar dados.\n");
        destruir_gestor_global(gestor);
        return 1;
    }
    
    printf("Dataset carregado...\n");
    
    char query_input[256];
    while (1) {
        printf("\nQue query deseja executar (1-6, ou 'q' para sair)? ");
        fflush(stdout);
        
        if (!fgets(query_input, sizeof(query_input), stdin)) {
            break;
        }
        limpar_newline(query_input);
        
        if (strcmp(query_input, "q") == 0 || strcmp(query_input, "Q") == 0) {
            break;
        }
        
        executar_query_interativa(gestor, query_input);
    }
    
    printf("\nA terminar...\n");
    destruir_gestor_global(gestor);
    
    return 0;
}
