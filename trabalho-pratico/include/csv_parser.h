#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>
#include <stddef.h>

typedef struct ParseResultado ParseResultado;

ParseResultado* parse_resultado_create(void);
void parse_resultado_mark_valid(ParseResultado *resultado, void *entidade);
bool parse_resultado_is_valid(const ParseResultado *resultado);
void* parse_resultado_get_entidade(const ParseResultado *resultado);
void parse_resultado_destroy(ParseResultado *resultado);

int csv_split_quoted(
    char *s,//linha que se transforma em tokens
    char sep,// o separador entre os tokens é a ','
    char **out,//array onde ficam os ponteiros para o inicio de cada campo; ex: iaca
    int max_out//número máximo de carateres
);
#endif