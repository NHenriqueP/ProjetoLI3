#include "aeroportos.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

struct Aeroporto {
    char *iata;
    char *nome;
    char *cidade;
    char *pais;
    char *latitude;
    char *longitude;
    char *icao;
    char *tipo;
};

Aeroporto* aeroporto_create(const char *iata,
                            const char *nome,
                            const char *cidade,
                            const char *pais,
                            const char *latitude,
                            const char *longitude,
                            const char *icao,
                            const char *tipo) {
    if (!iata || !nome || !cidade || !pais || !latitude || !longitude || !icao || !tipo) return NULL;

    Aeroporto *aeroporto = g_new0(Aeroporto, 1);
    if (!aeroporto) return NULL;

    aeroporto->iata = g_strdup(iata);
    aeroporto->nome = g_strdup(nome);
    aeroporto->cidade = g_strdup(cidade);
    aeroporto->pais = g_strdup(pais);
    aeroporto->latitude = g_strdup(latitude);
    aeroporto->longitude = g_strdup(longitude);
    aeroporto->icao = g_strdup(icao);
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
    g_free(aeroporto->latitude);
    g_free(aeroporto->longitude);
    g_free(aeroporto->icao);
    g_free(aeroporto->tipo);
    g_free(aeroporto);
}

const char* aeroporto_get_iata(const Aeroporto *aeroporto) {
    if (aeroporto) return aeroporto->iata;
    return NULL;
}

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

