#ifndef PARSER_H
#define PARSER_H
   

#include "aeroportos.h"
#include "aeronaves.h"
#include "passageiros.h"
#include "reservas.h"
#include "voos.h"

typedef struct {
    bool valido;
    Aeroporto *airport;
    const char *motivo;
} ParseResultado;

#endif // PARSER_H