#ifndef PARSER_AEROPORTO_H
#define PARSER_AEROPORTO_H

#include "csv_parser.h"
#include "aeroportos.h"

int verifica_coordenadas(const char *s, int digitos_max);
int validar_latitude(const char *s);
int validar_longitude(const char *s);
int tipo_aeroporto_valido(char *tipo);
int validar_iata(const char *s);
int validar_icao(const char *s);


ParseResultado parse_airport_line(char **fields, int n_fields);
int csv_parser_airports(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
);

#endif
