#ifndef PARSER_PASSAGEIRO_H
#define PARSER_PASSAGEIRO_H

#include "csv_parser.h"
#include "passageiros.h"

ParseResultado parse_passageiro_line(char **fields, int n_fields, SistemaGestor *gestor);

#endif
