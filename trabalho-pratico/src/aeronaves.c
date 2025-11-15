#include "aeronaves.h"

#include <glib.h>
#include <stdlib.h>

struct Aeronave {
    char *id;
    char *fabricante;
    char *modelo;
    int ano;
    int capacidade;
    int alcance;
};

Aeronave* aeronave_create(const char *id,
                          const char *fabricante,
                          const char *modelo,
                          int ano,
                          int capacidade,
                          int alcance) {
    if (!id || !fabricante || !modelo) return NULL;

    Aeronave *a = g_new0(Aeronave, 1);
    if (!a) return NULL;

    a->id = g_strdup(id);
    a->fabricante = g_strdup(fabricante);
    a->modelo = g_strdup(modelo);
    a->ano = ano;
    a->capacidade = capacidade;
    a->alcance = alcance;

    if (!a->id || !a->fabricante || !a->modelo) {
        aeronave_destroy(a);
        return NULL;
    }

    return a;
}

void aeronave_destroy(Aeronave *a) {
    if (!a) return;

    g_free(a->id);
    g_free(a->fabricante);
    g_free(a->modelo);
    g_free(a);
}

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
    return 0;
}

int aeronave_get_capacidade(const Aeronave *a) {
    if (a) return a->capacidade;
    return 0;
}

int aeronave_get_alcance(const Aeronave *a) {
    if (a) return a->alcance;
    return 0;
}

// Parser movido para parser_aeronave.c