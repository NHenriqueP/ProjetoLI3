#include "aeronaves.h"
#include <stdio.h>

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

// Parser movido para parser_aeronave.c