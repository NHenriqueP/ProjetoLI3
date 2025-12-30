#ifndef QUERIE6_H
#define QUERIE6_H

#include "gestorglobal.h"

/**
 * @file querie6.h
 * @brief API da Query 6.
 */

/**
 * @brief Executa a Query 6 a partir de uma linha de input.
 *
 * A Query 6 calcula o aeroporto mais reservado por nacionalidade, de acordo
 * com os parâmetros presentes na linha de comando.
 *
 * @param linha Linha de comando.
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ6(char linha[], SistemaGestor *gestor, int *Nout);

/**
 * @brief Liberta a cache interna usada pela Query 6.
 *
 * @return Nada (função `void`).
 */
void querie6_cleanup_cache(void);

#endif
