#include "parser_aeronave.h"

#include <ctype.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

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

ParseResultado parse_aeronave_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;

    (void)gestor;

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

    Aeronave *a = g_new0(Aeronave, 1);
    if (!a) {
        return r;
    }

    a->id = g_strdup(identifier);
    a->fabricante = g_strdup(manufacturer);
    a->modelo = g_strdup(model);
    a->ano = year;
    a->capacidade = capacity;
    a->alcance = range;

    if (!a->id || !a->fabricante || !a->modelo) {
        g_free(a->id);
        g_free(a->fabricante);
        g_free(a->modelo);
        g_free(a);
        return r;
    }

    r.valido = true;
    r.entidade = a;
    return r;
}
