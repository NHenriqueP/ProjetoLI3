#include "aeronaves.h"

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

// Parser movido para parser_aeronave.c