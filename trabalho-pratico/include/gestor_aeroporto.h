#ifndef GESTOR_AEROPORTO_H
#define GESTOR_AEROPORTO_H

#include <glib.h>
#include "aeroportos.h"

typedef struct aeroporto_ptr_manager AeroportoPtrManager;

/**
 * @file gestor_aeroporto.h
 * @brief Gestor de aeroportos (armazenamento e pesquisa).
 *
 * Define a API pública para gestão de aeroportos carregados do dataset.
 */

/**
 * @brief Cria um gestor de aeroportos.
 *
 * Inicializa as estruturas internas necessárias para armazenar e pesquisar
 * aeroportos.
 *
 * @return Ponteiro para o gestor criado, ou `NULL` em caso de falha.
 */
AeroportoPtrManager* airport_ptr_manager_create();

/**
 * @brief Destrói um gestor de aeroportos.
 *
 * Liberta o gestor e todos os recursos associados.
 *
 * @param manager Gestor a destruir.
 * @return Nada (função `void`).
 */
void airport_ptr_manager_destroy(AeroportoPtrManager *manager);

/**
 * @brief Adapter de inserção usado pelo parser genérico.
 *
 * Função com assinatura compatível com `csv_parse_file()`, que recebe um
 * ponteiro genérico para o gestor e para a entidade e realiza a inserção.
 *
 * @param gestor Ponteiro genérico para `AeroportoPtrManager`.
 * @param entidade Ponteiro para a entidade a inserir (tipicamente `Aeroporto*`).
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

/**
 * @brief Procura um aeroporto pelo seu código IATA.
 *
 * @param manager Gestor de aeroportos.
 * @param iata Código IATA a procurar.
 * @return Ponteiro para o aeroporto, ou `NULL` se não existir.
 */
Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata);

#endif