#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>

typedef struct {
    bool valido;
    void *entidade; // ponteiro específico: Aeroporto*, Voo*,etc...
    const char *motivo;
} ParseResultado;

// Função que vai dar ao gestor x a entidade correspondente
typedef int (*GestorAddFunc)(void *gestor, const void *entidade);

// Função parser de linha: recebe campos/n_campos e devolve ParseResultado
typedef ParseResultado (*LinhaParser)(char **fields, int n_fields);



#endif