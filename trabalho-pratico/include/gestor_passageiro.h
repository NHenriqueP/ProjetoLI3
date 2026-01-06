#ifndef GESTOR_PASSAGEIRO_H
#define GESTOR_PASSAGEIRO_H

#include <glib.h>
#include "passageiros.h"

typedef struct passageiro_ptr_manager PassageiroPtrManager;

/**
 * @file gestor_passageiro.h
 * @brief Gestor de passageiros (armazenamento e pesquisa).
 */

/**
 * @brief Cria um gestor de passageiros.
 *
 * @return Ponteiro para o gestor criado, ou `NULL` em caso de falha.
 */
PassageiroPtrManager* passageiro_ptr_manager_create();

/**
 * @brief Destrói um gestor de passageiros.
 *
 * @param manager Gestor a destruir.
 * @return Nada (função `void`).
 */
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager);

/**
 * @brief Adapter de inserção usado pelo parser genérico.
 *
 * @param gestor Ponteiro genérico para `PassageiroPtrManager`.
 * @param entidade Ponteiro para a entidade a inserir (tipicamente `Passageiro*`).
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

/**
 * @brief Verifica se existe um passageiro com um dado número de documento.
 *
 * @param manager Gestor de passageiros.
 * @param document_number Número de documento a procurar.
 * @return Valor inteiro indicando existência (0/1 conforme implementação).
 */
int existe_document_number(PassageiroPtrManager *manager, const char *document_number);

// Procura passageiro por document_number (retorna NULL se não existir)
/**
 * @brief Procura um passageiro por número de documento.
 *
 * @param manager Gestor de passageiros.
 * @param document_number Número de documento.
 * @return Ponteiro para o passageiro, ou `NULL` se não existir.
 */
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number);

#endif