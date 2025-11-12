#ifndef PARSER_AEROPORTO_H
#define PARSER_AEROPORTO_H

#include "csv_parser.h"
#include "aeroportos.h"

ParseResultado parse_airport_line(char **fields, int n_fields, SistemaGestor *gestor);

#endif
