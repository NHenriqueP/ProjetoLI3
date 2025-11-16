#include "parser_aeronave.h"
#include "aeronaves.h"
#include "gestor_aeronave.h"
#include <ctype.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "csv_parser.h"

static int validar_identifier(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    for (size_t i = 0; s[i] != '\0'; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != '-') return 0;
    }
    return 1;
}

static int validar_numero(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    char *endptr;
    long numero = strtol(s, &endptr, 10);
    if (*endptr != '\0' || numero < 0) return 0;
    return 1;
}

ParseResultado* parse_aeronave_line(char **fields, int n_fields) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;


    if (n_fields < 6) {
        return r;
    }

    char *identifier   = fields[0];
    char *manufacturer = fields[1];
    char *model        = fields[2];
    char *year_str     = fields[3];
    char *capacity_str = fields[4];
    char *range_str    = fields[5];

    if (!validar_identifier(identifier)) {
        return r;
    }

    if (!manufacturer || strlen(manufacturer) == 0) {
        return r;
    }

    if (!model || strlen(model) == 0) {
        return r;
    }

    if (!validar_numero(year_str) || !validar_numero(capacity_str) || !validar_numero(range_str)) {
        return r;
    }

    long year_long = strtol(year_str, NULL, 10);
    long capacity_long = strtol(capacity_str, NULL, 10);
    long range_long = strtol(range_str, NULL, 10);

    int year = (int)year_long;
    int capacity = (int)capacity_long;
    int range = (int)range_long;

    if (year < 1900 || year > 2100) {
        return r;
    }

    Aeronave *a = aeronave_create(identifier, manufacturer, model, year, capacity, range);
    if (!a) {
        return r;
    }

    parse_resultado_mark_valid(r, a);
    return r;
}

int csv_parser_aeronaves(
    const char *caminho_csv,       // Caminho do ficheiro a ler
    const char *ficheiro_erros,    // Caminho para o ficheiro de erros
    void *gestor,                  // Ponteiro para o gestor       // Função que faz o parsing da linha
    char separador                 // Separador CSV: ','
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(caminho_csv ? caminho_csv : "<NULL>", stderr);
        fputs("': ", stderr);//fecha a aspa e separa.
        fputs(strerror(errno), stderr);
        fputc('\n', stderr);
        return 0;
    }

    FILE *ficheiro_erros_ptr = NULL;
    char *linha = NULL;
    // getline() usa size_t para o tamanho  por isso o uso do size_t 
    size_t tam = 0;
    // 'lida' guarda o resultado de getline(): número de bytes lidos ou -1 em EOF/erro.
    // getline() retorna ssize_t (signed) para poder indicar -1, por isso usamos ssize_t.
    ssize_t lida;
    char *header = NULL;
    bool header_lido = false;
    int contador_validos = 0;

    while ((lida = getline(&linha, &tam, ficheiro)) != -1) {
        // Preservar a linha original (incluindo terminadores de linha) para o ficheiro de erros
        size_t original_len = (size_t)lida;
        char *linha_original = g_strndup(linha, original_len);

        if (header_lido) {
        // Remover \n e \r para efeitos de parsing (sem afetar o comprimento usado para escrever no ficheiro de erros)
        while (lida > 0 && (linha[lida - 1] == '\n' || linha[lida - 1] == '\r')) // \r serve para compatibilidade com Windows
            linha[--lida] = '\0';
        }
        // Guardar o cabeçalho (primeira linha) e continuar
        if (!header_lido) {
            header = g_strdup(linha);
            header_lido = true;
            // já não precisamos da cópia da linha original nesta iteração
            g_free(linha_original);
            continue;
        }

        // Dividir linha em campos (suporta aspas e vírgulas dentro de aspas)
        char *fields[15];
        int max_fields = 15;
        int n_fields = csv_split_quoted(linha, separador, fields, max_fields);

        // Validar linha com o parser específico fornecido
        ParseResultado *resultado = parse_aeronave_line(fields, n_fields);

        if (!resultado || !parse_resultado_is_valid(resultado)) {
            // Cria o ficheiro de erros se ainda não existir
            if (!ficheiro_erros_ptr) {
                ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
                fwrite(header, 1, strlen(header), ficheiro_erros_ptr);
                if (!ficheiro_erros_ptr) {
                    fputs("Erro ao criar ficheiro de erros: ", stderr);
                    fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
                    fputc('\n', stderr);
                    if (resultado) parse_resultado_destroy(resultado);
                    g_free(linha_original);
                    break;
                }
            }
            // Escreve a linha inválida exatamente como lida (com terminadores originais)
            if (linha_original) {
                fwrite(linha_original, 1, original_len, ficheiro_erros_ptr);
            } else {
                // fallback: escreve versão já normalizada
                fputs(linha, ficheiro_erros_ptr);
                fputc('\n', ficheiro_erros_ptr);
            }
            if (resultado) parse_resultado_destroy(resultado);
        } else {
            // Adiciona entidade válida ao gestor
            aeronave_ptr_manager_add_from_ptr_adapter(gestor, parse_resultado_get_entidade(resultado));
            contador_validos++;
            parse_resultado_destroy(resultado);
        }

        // libertar cópia da linha
        g_free(linha_original);
    }


    if (ficheiro_erros_ptr) fclose(ficheiro_erros_ptr);
    g_free(header);
    free(linha);
    fclose(ficheiro);

    return contador_validos;
}