#ifndef PARSER_VOO_H
#define PARSER_VOO_H

#include "csv_parser.h"
#include "voos.h"

ParseResultado parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor);

#endif
