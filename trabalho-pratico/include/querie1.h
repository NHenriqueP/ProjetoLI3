#include "gestorglobal.h"
#include "aeroportos.h"

void execQ1(char linha[] ,SistemaGestor *gestor,int *Nout);

// Libertar cache interna da Q1 (para evitar leaks em testes/valgrind)
void querie1_cleanup_cache(void);