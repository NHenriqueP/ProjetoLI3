#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>
#include <stddef.h>
#include "gestorglobal.h"

typedef struct parse_resultado {
    bool valido;
    void *entidade;
} ParseResultado;

int csv_split_quoted(
    char *s,//linha que se transforma em tokens
    char sep,// o separador entre os tokens é a ','
    char **out,//array onde ficam os ponteiros para o inicio de cada campo; ex: iaca
    int max_out//número máximo de carateres
);

#endif