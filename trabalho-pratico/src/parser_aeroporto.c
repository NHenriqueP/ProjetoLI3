#include "parser_aeroporto.h"

#include <glib.h>
#include <string.h>

ParseResultado parse_airport_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;

    (void)gestor;

    if (n_fields < 8) {
        return r;
    }

    char *iata      = fields[0];
    char *nome      = fields[1];
    char *cidade    = fields[2];
    char *pais      = fields[3];
    char *latitude  = fields[4];
    char *longitude = fields[5];
    char *icao      = fields[6];
    char *tipo      = fields[7];

    if (!validar_iata(iata) || !validar_icao(icao)) {
        return r;
    }

    if (!validar_latitude(latitude) || !validar_longitude(longitude)) {
        return r;
    }

    if (!nome || !cidade || !pais || strlen(nome) == 0 || strlen(cidade) == 0 || strlen(pais) == 0) {
        return r;
    }

    if (tipo_aeroporto_valido(tipo) == 0) {
        return r;
    }

    Aeroporto *a = g_new0(Aeroporto, 1);
    if (!a) {
        return r;
    }

    a->iata      = g_strdup(iata);
    a->nome      = g_strdup(nome);
    a->cidade    = g_strdup(cidade);
    a->pais      = g_strdup(pais);
    a->latitude  = g_strdup(latitude);
    a->longitude = g_strdup(longitude);
    a->icao      = g_strdup(icao);
    a->tipo      = g_strdup(tipo);

    if (!a->iata || !a->nome || !a->cidade || !a->pais ||
        !a->latitude || !a->longitude || !a->icao || !a->tipo) {
        g_free(a->iata);
        g_free(a->nome);
        g_free(a->cidade);
        g_free(a->pais);
        g_free(a->latitude);
        g_free(a->longitude);
        g_free(a->icao);
        g_free(a->tipo);
        g_free(a);
        return r;
    }

    r.valido = true;
    r.entidade = a;
    return r;
}
