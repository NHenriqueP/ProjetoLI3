#ifndef QUERIE5_H
#define QUERIE5_H

#include "gestorglobal.h"

// Query 5: Top N companhias com mais tempo de atraso médio por voo (apenas DELAYED)
// Formato do input: "5[S] <N>" (com S opcional para usar '=' como separador)
// Output: airline<sep>delayed_flights_count<sep>average_delay (3 casas decimais)
void execQ5(char linha[], SistemaGestor *gestor, int *Nout);

#endif
