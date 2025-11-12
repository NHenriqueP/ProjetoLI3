#ifndef PARSER_RESERVA_H
#define PARSER_RESERVA_H

#include "csv_parser.h"
#include "reservas.h"

ParseResultado parse_reserva_line(char **fields, int n_fields, SistemaGestor *gestor);

#endif
