#ifndef QUERIE4_H
#define QUERIE4_H

#include "gestorglobal.h"

// Query 4: Passageiro que esteve mais vezes no top 10 semanal por gastos.
// Input: "4[S] [begin_date end_date]" (datas opcionais, formato YYYY-MM-DD)
// Output: document_number;first_name;last_name;dob;nationality;count_top_10
void execQ4(char linha[], SistemaGestor *gestor, int *Nout);

// Libertar cache interna da Q4 (para evitar leaks em testes/valgrind)
void querie4_cleanup_cache(void);

#endif
