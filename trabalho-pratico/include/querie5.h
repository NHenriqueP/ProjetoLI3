#ifndef QUERIE5_H
#define QUERIE5_H

#include "gestorglobal.h"

/**
 * @file querie5.h
 * @brief API da Query 5.
 */

/**
 * @brief Executa a Query 5 a partir de uma linha de input.
 *
 * A Query 5 calcula o top N de companhias com maior atraso médio por voo
 * (tipicamente considerando apenas voos com estado DELAYED). O input pode
 * incluir um sufixo opcional para indicar o separador a usar.
 *
 * @param linha Linha de comando (ex.: "5[S] <N>").
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ5(char linha[], SistemaGestor *gestor, int *Nout);

/**
 * @brief Liberta a cache interna usada pela Query 5.
 *
 * Deve ser chamada entre execuções/testes caso a query mantenha estado
 * interno para reutilização.
 *
 * @return Nada (função `void`).
 */
void querie5_cleanup_cache(void);

#endif
