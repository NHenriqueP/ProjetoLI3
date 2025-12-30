#ifndef QUERIE3_H
#define QUERIE3_H

#include "gestorglobal.h"

/**
 * @file querie3.h
 * @brief API da Query 3.
 */

/**
 * @brief Executa a Query 3 a partir de uma linha de input.
 *
 * Interpreta os argumentos na linha de comando e escreve os resultados no
 * ficheiro de output associado ao índice `Nout`.
 *
 * @param linha Linha de comando.
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ3(char linha[], SistemaGestor *gestor, int *Nout);

#endif