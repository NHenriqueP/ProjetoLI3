#ifndef PARSER_RESERVA_H
#define PARSER_RESERVA_H

#include "csv_parser.h"
#include "reservas.h"
#include "gestorglobal.h"

ParseResultado* parse_reserva_line(char **fields, int n_fields, SistemaGestor *gestor);
void* criar_reserva(char **fields, int n_fields, void *ctx);

int csv_parser_reservas(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    SistemaGestor *gestorGlob,
    char separador
);

#endif