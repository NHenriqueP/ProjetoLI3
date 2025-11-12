#include "aeroportos.h"
#include <stdio.h>

// Produz um resumo CSV do aeroporto (IATA,Nome,Cidade,País,Tipo)
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize) {
    if (!airport || !buf || bufsize == 0) return 0;
    int n = snprintf(buf, bufsize, "%s,%s,%s,%s,%s",
                     airport->iata ? airport->iata : "",
                     airport->nome ? airport->nome : "",
                     airport->cidade ? airport->cidade : "",
                     airport->pais ? airport->pais : "",
                     airport->tipo ? airport->tipo : "");
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

/* Parser movido para parser_aeroporto.c */