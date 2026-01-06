#include "aeroportos.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

struct Aeroporto {
    char *iata;
    char *nome;
    char *cidade;
    char *pais;
    char *tipo;
};

Aeroporto* aeroporto_create(const char *iata,
                            const char *nome,
                            const char *cidade,
                            const char *pais,
                            const char *tipo) {
    if (!iata || !nome || !cidade || !pais || !tipo) return NULL;

    Aeroporto *aeroporto = g_new0(Aeroporto, 1);
    if (!aeroporto) return NULL;

    aeroporto->iata = g_strdup(iata);
    aeroporto->nome = g_strdup(nome);
    aeroporto->cidade = g_strdup(cidade);
    aeroporto->pais = g_strdup(pais);
    aeroporto->tipo = g_strdup(tipo);

    if (!aeroporto->iata) {
        aeroporto_destroy(aeroporto);
        return NULL;
    }

    return aeroporto;
}

void aeroporto_destroy(Aeroporto *aeroporto) {
    if (!aeroporto) return;

    g_free(aeroporto->iata);
    g_free(aeroporto->nome);
    g_free(aeroporto->cidade);
    g_free(aeroporto->pais);
    g_free(aeroporto->tipo);
    g_free(aeroporto);
}

const char* aeroporto_get_iata(const Aeroporto *aeroporto) {
    if (aeroporto) return aeroporto->iata;
    return NULL;
}

int airport_summary_delim(const Aeroporto *airport, char *buf, size_t bufsize, char sep) {
    if (!airport || !buf || bufsize == 0) return 0;
    int n = snprintf(buf, bufsize, "%s%c%s%c%s%c%s%c%s",
                     airport->iata ? airport->iata : "", sep,
                     airport->nome ? airport->nome : "", sep,
                     airport->cidade ? airport->cidade : "", sep,
                     airport->pais ? airport->pais : "", sep,
                     airport->tipo ? airport->tipo : "");
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

