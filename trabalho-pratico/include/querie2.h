#ifndef QUERIE2_H
#define QUERIE2_H

#include "gestorglobal.h"

// Executa a Query 2 com base na linha de input:
// "2 <N> [<fabricante>]"
// Escreve resultados em resultados/command<Nout>_output.txt
void execQ2(char linha[], SistemaGestor *gestor, int *Nout);

#endif
