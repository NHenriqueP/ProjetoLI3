#include "aeronaves.h"
#include "csv_parser.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// ==================== GETTERS ====================

const char* aeronave_get_id(const Aeronave *a) {
    if (a) return a->id;
    return NULL;
}

const char* aeronave_get_fabricante(const Aeronave *a) {
    if (a) return a->fabricante;
    return NULL;
}

const char* aeronave_get_modelo(const Aeronave *a) {
    if (a) return a->modelo;
    return NULL;
}

int aeronave_get_ano(const Aeronave *a) {
    if (a) return a->ano;
    return -1;
}

int aeronave_get_capacidade(const Aeronave *a) {
    if (a) return a->capacidade;
    return -1;
}

int aeronave_get_alcance(const Aeronave *a) {
    if (a) return a->alcance;
    return -1;
}

// Produz um resumo CSV da aeronave
int aeronave_summary(const Aeronave *aeronave, char *buf, size_t bufsize) {
    if (!aeronave || !buf || bufsize == 0) return 0;
    
    int n = snprintf(buf, bufsize, "%s,%s,%s,%d,%d,%d",
                     aeronave->id ? aeronave->id : "",
                     aeronave->fabricante ? aeronave->fabricante : "",
                     aeronave->modelo ? aeronave->modelo : "",
                     aeronave->ano,
                     aeronave->capacidade,
                     aeronave->alcance);
    
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

// ==================== CSV PARSER ====================

// Valida se o identifier é válido (alfanumérico com possíveis hífens)
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
    long numero = strtol(s, &endptr, 10); // Apenas verifica se é um número válido
    
    
    if (*endptr != '\0' || numero < 0) return 0;

    return 1;
}

// Parser de linha CSV para aeronaves
ParseResultado parse_aeronave_line(char **fields, int n_fields, SistemaGestor* gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    
    // Esperamos 6 campos: identifier, manufacturer, model, year, capacity, range
    if (n_fields < 6) {
        return r;
    }
    
    char *identifier   = fields[0];
    char *manufacturer = fields[1];
    char *model        = fields[2];
    char *year_str     = fields[3];
    char *capacity_str = fields[4];
    char *range_str    = fields[5];
    
    // Validações
    if (!validar_identifier(identifier)) {
        return r;
    }
    
    if (!manufacturer || strlen(manufacturer) == 0) {
        return r;
    }
    
    if (!model || strlen(model) == 0) {
        return r;
    }
    
    if (!validar_numero(year_str)) {
        return r;
    }
    
    if (!validar_numero(capacity_str)) {
        return r;
    }
    
    if (!validar_numero(range_str)) {
        return r;
    }
    
    long year_long = strtol(year_str, NULL, 10);
    long capacity_long = strtol(capacity_str, NULL, 10);
    long range_long = strtol(range_str, NULL, 10);

    int year = (int)year_long;
    int capacity = (int)capacity_long;
    int range = (int)range_long;

    if (year <1900 || year > 2100) {
        return r;
    }
    
    // Criar aeronave
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