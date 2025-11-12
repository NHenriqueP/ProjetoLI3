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

int aeronave_summary(const Aeronave *aeronave, char *buf, size_t bufsize);

const char* aeronave_get_id(const Aeronave *a);
const char* aeronave_get_fabricante(const Aeronave *a);
const char* aeronave_get_modelo(const Aeronave *a);
int aeronave_get_ano(const Aeronave *a);
int aeronave_get_capacidade(const Aeronave *a);
int aeronave_get_alcance(const Aeronave *a);

#endif