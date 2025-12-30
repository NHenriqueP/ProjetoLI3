#ifndef QUERIE2_H
#define QUERIE2_H

#include "gestorglobal.h"

/**
 * @file querie2.h
 * @brief API da Query 2.
 */

/**
 * @brief Executa a Query 2 a partir de uma linha de input.
 *
 * Interpreta uma linha do tipo "2 <N> [<fabricante>]" e escreve os
 * resultados no ficheiro `resultados/command<Nout>_output.txt`.
 *
 * @param linha Linha de comando.
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ2(char linha[], SistemaGestor *gestor, int *Nout);

#endif