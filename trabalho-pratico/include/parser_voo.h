#ifndef PARSER_VOO_H
#define PARSER_VOO_H

#include "csv_parser.h"
#include "voos.h"
#include "gestorglobal.h"

ParseResultado* parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor);

int csv_parser_voos(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    SistemaGestor *sistema_gestor,
    char separador
);

#endif