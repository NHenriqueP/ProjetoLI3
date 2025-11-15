#ifndef AERONAVE
#define AERONAVE

#include <stddef.h>

typedef struct aeronave {
    char *id;
    char *fabricante;
    char *modelo;
    int ano;
    int capacidade;
    int alcance;
} Aeronave;

const char* aeronave_get_id(const Aeronave *a);
const char* aeronave_get_fabricante(const Aeronave *a);
const char* aeronave_get_modelo(const Aeronave *a);

#endif