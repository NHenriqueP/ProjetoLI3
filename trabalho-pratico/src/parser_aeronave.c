#include "parser_aeronave.h"
#include "aeronaves.h"
#include "gestor_aeronave.h"
#include <ctype.h>
#include <errno.h>
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

void* criar_aeronave(char **fields, int n_fields, void *ctx) {
    (void)ctx;
    if (n_fields < 6) return NULL;

    char *identifier   = fields[0];
    char *manufacturer = fields[1];
    char *model        = fields[2];
    char *year_str     = fields[3];
    char *capacity_str = fields[4];
    char *range_str    = fields[5];

    if (!validar_identifier(identifier)) return NULL;
    if (!manufacturer || strlen(manufacturer) == 0) return NULL;
    if (!model || strlen(model) == 0) return NULL;
    if (!validar_numero(year_str) || !validar_numero(capacity_str) || !validar_numero(range_str)) return NULL;

    long year_long = strtol(year_str, NULL, 10);
    long capacity_long = strtol(capacity_str, NULL, 10);
    long range_long = strtol(range_str, NULL, 10);

    int year = (int)year_long;
    int capacity = (int)capacity_long;
    int range = (int)range_long;

    if (year < 1900 || year > 2100) return NULL;

    return aeronave_create(identifier, manufacturer, model, year, capacity, range);
}

ParseResultado* parse_aeronave_line(char **fields, int n_fields) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    Aeronave *a = (Aeronave*)criar_aeronave(fields, n_fields, NULL);
    if (!a) return r;

    parse_resultado_mark_valid(r, a);
    return r;
}

int csv_parser_aeronaves(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(caminho_csv ? caminho_csv : "<NULL>", stderr);
        fputs("': ", stderr);
        fputs(strerror(errno), stderr);
        fputc('\n', stderr);
        return 0;
    }

    FILE *ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
    if (!ficheiro_erros_ptr) {
        fputs("Erro ao criar ficheiro de erros: ", stderr);
        fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
        fputc('\n', stderr);
        fclose(ficheiro);
        return 0;
    }

    int contador_validos = csv_generic_parser(
        ficheiro,
        ficheiro_erros_ptr,
        gestor,
        NULL,
        NULL,
        criar_aeronave,
        aeronave_ptr_manager_add_from_ptr_adapter,
        separador
    );

    fclose(ficheiro_erros_ptr);
    fclose(ficheiro);

    return contador_validos;
}