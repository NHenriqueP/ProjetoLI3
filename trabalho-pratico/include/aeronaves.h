#ifndef AERONAVE
#define AERONAVE

typedef struct Aeronave Aeronave;

/**
 * @file aeronaves.h
 * @brief API pública da entidade Aeronave.
 */

/**
 * @brief Cria uma aeronave.
 *
 * Copia internamente os campos fornecidos.
 *
 * @param id Identificador da aeronave.
 * @param fabricante Fabricante.
 * @param modelo Modelo.
 * @return Ponteiro para a aeronave criada, ou `NULL` em caso de falha.
 */
Aeronave* aeronave_create(const char *id,
						  const char *fabricante,
						  const char *modelo);

/**
 * @brief Destrói uma aeronave.
 *
 * @param a Aeronave a destruir.
 * @return Nada (função `void`).
 */
void aeronave_destroy(Aeronave *a);

/**
 * @brief Obtém o identificador da aeronave.
 *
 * @param a Aeronave.
 * @return String com o id (propriedade da aeronave; não libertar).
 */
const char* aeronave_get_id(const Aeronave *a);

/**
 * @brief Obtém o fabricante da aeronave.
 *
 * @param a Aeronave.
 * @return String com o fabricante (propriedade da aeronave; não libertar).
 */
const char* aeronave_get_fabricante(const Aeronave *a);

/**
 * @brief Obtém o modelo da aeronave.
 *
 * @param a Aeronave.
 * @return String com o modelo (propriedade da aeronave; não libertar).
 */
const char* aeronave_get_modelo(const Aeronave *a);

#endif