#ifndef AERONAVE
#define AERONAVE

typedef struct Aeronave Aeronave;

Aeronave* aeronave_create(const char *id,
						  const char *fabricante,
						  const char *modelo,
						  int ano,
						  int capacidade,
						  int alcance);

void aeronave_destroy(Aeronave *a);

const char* aeronave_get_id(const Aeronave *a);
const char* aeronave_get_fabricante(const Aeronave *a);
const char* aeronave_get_modelo(const Aeronave *a);

#endif