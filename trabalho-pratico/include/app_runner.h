#ifndef APP_RUNNER_H
#define APP_RUNNER_H

#include "gestorglobal.h"

int carregar_dados(const char *pasta_csv, SistemaGestor *gestor);
int executar_querry(char linha[], SistemaGestor *gestor, int *Nout);
int ler_inputs(const char *caminho_inputs, SistemaGestor *gestor, int *nOut);

#endif
