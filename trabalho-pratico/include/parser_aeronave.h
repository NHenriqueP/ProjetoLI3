#ifndef PARSER_AERONAVE_H
#define PARSER_AERONAVE_H

#include "csv_parser.h"
#include "aeronaves.h"

ParseResultado parse_aeronave_line(char **fields, int n_fields, SistemaGestor *gestor);

#endif
