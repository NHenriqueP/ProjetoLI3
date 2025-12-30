#ifndef QUERIE4_H
#define QUERIE4_H

#include "gestorglobal.h"

/**
 * @file querie4.h
 * @brief API da Query 4.
 */

/**
 * @brief Executa a Query 4 a partir de uma linha de input.
 *
 * A Query 4 calcula o passageiro que esteve mais vezes no top 10 semanal por
 * gastos. O input pode incluir datas opcionais (formato YYYY-MM-DD).
 *
 * @param linha Linha de comando (ex.: "4[S] [begin_date end_date]").
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Ponteiro para o índice/contador de outputs.
 * @return Nada (função `void`).
 */
void execQ4(char linha[], SistemaGestor *gestor, int *Nout);

/**
 * @brief Liberta a cache interna usada pela Query 4.
 *
 * Útil para evitar fugas aparentes quando a query mantém estruturas estáticas
 * para otimização.
 *
 * @return Nada (função `void`).
 */
void querie4_cleanup_cache(void);

#endif
