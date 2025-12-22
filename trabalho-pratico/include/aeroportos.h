#ifndef AEROPORTO
#define AEROPORTO
#include <stddef.h> // size_t

typedef struct Aeroporto Aeroporto;

Aeroporto* aeroporto_create(const char *iata,
                         	   const char *nome,
                            const char *cidade,
                            const char *pais,
                            const char *tipo);

void aeroporto_destroy(Aeroporto *aeroporto);

const char* aeroporto_get_iata(const Aeroporto *aeroporto);

// Resumo formatado de um aeroporto no buffer fornecido
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize);

// Resumo formatado de um aeroporto no buffer fornecido, usando um separador específico
// Produz: IATA<sep>Nome<sep>Cidade<sep>País<sep>Tipo
int airport_summary_delim(const Aeroporto *airport, char *buf, size_t bufsize, char sep);

#endif 