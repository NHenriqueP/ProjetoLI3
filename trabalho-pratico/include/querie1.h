#ifndef QUERIE1_H
#define QUERIE1_H

#include "gestorglobal.h"

/**
 * @file querie1.h
 * @brief API da Query 1.
 *
 * Este header expõe a função de execução da Query 1 e uma função opcional
 * de limpeza de cache interna (útil em testes/valgrind).
 */

/**
 * @brief Executa a Query 1 a partir de uma linha de input.
 *
 * A função interpreta os argumentos presentes na linha de comando e escreve
 * o resultado no ficheiro de output associado ao índice `Nout`.
 *
 * @param linha Linha de comando (ex.: começa por "1").
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ1(char linha[] ,SistemaGestor *gestor,int *Nout);

/**
 * @brief Liberta a cache interna usada pela Query 1.
 *
 * Deve ser chamada no fim do programa (ou entre testes) para libertar
 * estruturas estáticas usadas para otimização.
 *
 * @return Nada (função `void`).
 */
void querie1_cleanup_cache(void);

#endif