#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>
#include <stddef.h>
#include "gestorglobal.h"

typedef struct parse_resultado {
    bool valido;
    void *entidade;
} ParseResultado;

// Função que vai dar ao gestor x a entidade correspondente
typedef int (*GestorAddFunc)(void *gestor, const void *entidade);

// Função parser de linha: recebe campos/n_campos e devolve ParseResultado
typedef ParseResultado (*LinhaParser)(char **fields, int n_fields, SistemaGestor *gestor);

// Função principal do parser genérico de CSV.
// Retorna o número de linhas válidas adicionadas ao gestor.
int csv_parser(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    void *gestorGlob,
    LinhaParser parser_func,
    GestorAddFunc add_func,
    char separador
);

// Validadores auxiliares expostos (úteis para outros módulos)
int validar_iata(const char *s);
int validar_icao(const char *s);
int validar_latitude(const char *s);
int validar_longitude(const char *s);
int tipo_aeroporto_valido(char *tipo);


#endif