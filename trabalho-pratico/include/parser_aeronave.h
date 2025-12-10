#ifndef PARSER_AERONAVE_H
#define PARSER_AERONAVE_H

#include "csv_parser.h"
#include "aeronaves.h"


ParseResultado* parse_aeronave_line(char **fields, int n_fields);
void* criar_aeronave(char **fields, int n_fields, void *ctx);
int csv_parser_aeronaves(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
);

#endif