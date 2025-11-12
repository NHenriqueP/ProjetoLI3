#ifndef AEROPORTO
#define AEROPORTO

#include <stddef.h> // size_t

typedef struct aeroporto {
	char *iata;
	char *nome;
	char *cidade;
	char *pais;
	char *latitude;
	char *longitude;
	char *icao;
	char *tipo;
} Aeroporto;

// Resumo formatado de um aeroporto no buffer fornecido
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize);

#endif 