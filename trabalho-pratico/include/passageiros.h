#ifndef PASSAGEIRO
#define PASSAGEIRO

#include <stddef.h>

typedef struct Passageiro Passageiro;

/**
 * @file passageiros.h
 * @brief API pública da entidade Passageiro.
 */

/**
 * @brief Cria um passageiro.
 *
 * Copia internamente os campos fornecidos.
 *
 * @param document_number Número de documento do passageiro.
 * @param first_name Primeiro nome.
 * @param last_name Apelido.
 * @param dob Data de nascimento (string).
 * @param nationality Nacionalidade.
 * @return Ponteiro para o passageiro criado, ou `NULL` em caso de falha.
 */
Passageiro* passageiro_create(
	const char *document_number,
	const char *first_name,
	const char *last_name,
	const char *dob,
	const char *nationality
);

/**
 * @brief Destrói um passageiro.
 *
 * @param passageiro Passageiro a destruir.
 * @return Nada (função `void`).
 */
void passageiro_destroy(Passageiro *passageiro);

/**
 * @brief Obtém o número de documento do passageiro.
 *
 * @param passageiro Passageiro.
 * @return String com o número de documento (propriedade do passageiro; não libertar).
 */
const char* get_passageiro_document_number(const Passageiro *passageiro);

/**
 * @brief Obtém o primeiro nome do passageiro.
 *
 * @param passageiro Passageiro.
 * @return String com o primeiro nome (propriedade do passageiro; não libertar).
 */
const char* passageiro_get_first_name(const Passageiro *passageiro);

/**
 * @brief Obtém o apelido do passageiro.
 *
 * @param passageiro Passageiro.
 * @return String com o apelido (propriedade do passageiro; não libertar).
 */
const char* passageiro_get_last_name(const Passageiro *passageiro);

/**
 * @brief Obtém a data de nascimento do passageiro.
 *
 * @param passageiro Passageiro.
 * @return String com a data de nascimento (propriedade do passageiro; não libertar).
 */
const char* passageiro_get_dob(const Passageiro *passageiro);

/**
 * @brief Obtém a nacionalidade do passageiro.
 *
 * @param passageiro Passageiro.
 * @return String com a nacionalidade (propriedade do passageiro; não libertar).
 */
const char* passageiro_get_nationality(const Passageiro *passageiro);

#endif