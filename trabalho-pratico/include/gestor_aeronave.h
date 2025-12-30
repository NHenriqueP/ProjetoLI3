#ifndef GESTOR_AERONAVE_H
#define GESTOR_AERONAVE_H

#include <glib.h>
#include "aeronaves.h"

typedef struct aeronave_ptr_manager AeronavePtrManager;

/**
 * @file gestor_aeronave.h
 * @brief Gestor de aeronaves (armazenamento e acesso).
 */

/**
 * @brief Cria um gestor de aeronaves.
 *
 * @return Ponteiro para o gestor criado, ou `NULL` em caso de falha.
 */
AeronavePtrManager* aeronave_ptr_manager_create();

/**
 * @brief Destrói um gestor de aeronaves.
 *
 * @param manager Gestor a destruir.
 * @return Nada (função `void`).
 */
void aeronave_ptr_manager_destroy(AeronavePtrManager *manager);

/**
 * @brief Adapter de inserção usado pelo parser genérico.
 *
 * @param gestor Ponteiro genérico para `AeronavePtrManager`.
 * @param entidade Ponteiro para a entidade a inserir (tipicamente `Aeronave*`).
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

/**
 * @brief Número total de aeronaves carregadas.
 *
 * @param manager Gestor de aeronaves.
 * @return Número de aeronaves.
 */
guint aeronave_ptr_manager_count(const AeronavePtrManager *manager);

/**
 * @brief Acesso indexado a uma aeronave.
 *
 * @param manager Gestor de aeronaves.
 * @param index Índice (0..count-1).
 * @return Ponteiro para a aeronave no índice, ou `NULL` se o índice for inválido.
 */
const Aeronave* aeronave_ptr_manager_get(const AeronavePtrManager *manager, guint index);

/**
 * @brief Verifica se existe uma aeronave com um dado id.
 *
 * @param manager Gestor de aeronaves.
 * @param id Identificador da aeronave.
 * @return Valor inteiro indicando existência (0/1 conforme implementação).
 */
int existe_aeronave(AeronavePtrManager *manager, const char *id);

#endif