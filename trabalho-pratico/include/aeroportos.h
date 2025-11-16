#ifndef AEROPORTO
#define AEROPORTO
#include <stddef.h> // size_t

typedef struct Aeroporto Aeroporto;

Aeroporto* aeroporto_create(const char *iata,
                         	   const char *nome,
                            const char *cidade,
                            const char *pais,
                            const char *latitude,
                            const char *longitude,
                            const char *icao,
                            const char *tipo);

void aeroporto_destroy(Aeroporto *aeroporto);

const char* aeroporto_get_iata(const Aeroporto *aeroporto);

// Resumo formatado de um aeroporto no buffer fornecido
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize);

#endif 