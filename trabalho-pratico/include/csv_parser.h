#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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

int csv_generic_parser(
    FILE *data_file,
    FILE *error_file,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
);

int csv_parse_file(
    const char *data_path,
    const char *error_path,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
);

#endif