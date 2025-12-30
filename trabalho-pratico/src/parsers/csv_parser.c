#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "csv_parser.h"

struct ParseResultado {
    bool valido;
    void *entidade;
};

ParseResultado* parse_resultado_create(void) {
    ParseResultado *resultado = malloc(sizeof(ParseResultado));
    if (!resultado) return NULL;

    resultado->valido = false;
    resultado->entidade = NULL;
    return resultado;
}

void parse_resultado_mark_valid(ParseResultado *resultado, void *entidade) {
    if (!resultado) return;

    resultado->valido = true;
    resultado->entidade = entidade;
}

bool parse_resultado_is_valid(const ParseResultado *resultado) {
    if (!resultado) return false;
    return resultado->valido;
}

void* parse_resultado_get_entidade(const ParseResultado *resultado) {
    if (!resultado) return NULL;
    return resultado->entidade;
}

void parse_resultado_destroy(ParseResultado *resultado) {
    if (!resultado) return;
    free(resultado);
}

int csv_split_quoted(
    char *s,//linha que se transforma em tokens
    char sep,// o separador entre os tokens é a ','
    char **out,//array onde ficam os ponteiros para o inicio de cada campo; ex: iaca
    int max_out//número máximo de carateres
) 
{
    if (!s || !out || max_out <= 0) return 0;
    int n = 0;
    int inq = 0;
    char *r = s;
    char *w = s;
    out[n++] = w;
    while (*r) {
        char c = *r;
        if (c == '"') {
            if (inq && r[1] == '"') { 
                *w++ = '"';
                r += 2;
                continue;
            }
            inq = !inq; 
            r++;
            continue;
        }
        if (!inq && c == sep) {
            *w++ = '\0';
            if (n < max_out) out[n++] = w;
            r++;
            continue;
        }
        *w++ = c;
        r++;
    }
    *w = '\0';
    return n;
}
int csv_generic_parser(
    FILE *data_file,
    FILE *error_file,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
) {
    if (!data_file || !create_entidade || !insert_entidade) return 0;

    char *linha = NULL;
    size_t tam = 0;
    ssize_t lida;
    char *header = NULL;
    bool header_lido = false;
    bool header_written_to_error = false;
    int contador_validos = 0;

    while ((lida = getline(&linha, &tam, data_file)) != -1) {
        char *linha_original = strdup(linha);

        ssize_t len = lida;
        while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r')) {
            linha[--len] = '\0';
        }

        if (!header_lido) {
            header = strdup(linha_original);
            header_lido = true;
            free(linha_original);
            continue;
        }

        char *fields[20];
        int max_fields = 20;
        int n_fields = csv_split_quoted(linha, separador, fields, max_fields);

        bool valid = true;
        if (is_valid_entidade) {
            valid = is_valid_entidade(fields, n_fields, ctx);
        }

        void *entidade = NULL;
        if (valid && create_entidade) {
            entidade = create_entidade(fields, n_fields, ctx);
            if (!entidade) valid = false;
        }

        if (valid) {
            insert_entidade(gestor, entidade);
            contador_validos++;
        } else if (error_file) {
            if (!header_written_to_error && header) {
                fwrite(header, 1, strlen(header), error_file);
                header_written_to_error = true;
            }
            fwrite(linha_original, 1, strlen(linha_original), error_file);
        }

        free(linha_original);
    }

    free(linha);
    if (header) free(header);

    return contador_validos;
}

int csv_parse_file(
    const char *data_path,
    const char *error_path,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
) {
    if (!data_path || !error_path || !gestor || !create_entidade || !insert_entidade) return -1;

    FILE *data_file = fopen(data_path, "r");
    if (!data_file) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(data_path, stderr);
        fputs("'\n", stderr);
        return -1;
    }

    FILE *error_file = fopen(error_path, "w");
    if (!error_file) {
        fputs("Erro ao criar ficheiro de erros '", stderr);
        fputs(error_path, stderr);
        fputs("'\n", stderr);
        fclose(data_file);
        return -1;
    }

    int res = csv_generic_parser(
        data_file,
        error_file,
        gestor,
        ctx,
        is_valid_entidade,
        create_entidade,
        insert_entidade,
        separador
    );

    fclose(error_file);
    fclose(data_file);
    return res;
}
