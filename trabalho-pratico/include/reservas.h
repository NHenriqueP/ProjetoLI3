#ifndef RESERVAS
#define RESERVAS

#include <stdbool.h>

typedef struct Reserva Reserva;

/**
 * @file reservas.h
 * @brief API pública da entidade Reserva.
 */

/**
 * @brief Cria uma reserva.
 *
 * Copia internamente os campos fornecidos.
 *
 * @param reservation_id Identificador da reserva.
 * @param flight_ids Lista de ids de voos (string tal como no CSV).
 * @param document_number Número de documento do passageiro associado.
 * @param price Preço total da reserva.
 * @return Ponteiro para a reserva criada, ou `NULL` em caso de falha.
 */
Reserva* reserva_create(const char *reservation_id,
                        const char *flight_ids,
                        const char *document_number,
                        double price);

/**
 * @brief Destrói uma reserva.
 *
 * @param reserva Reserva a destruir.
 * @return Nada (função `void`).
 */
void reserva_destroy(Reserva *reserva);

/**
 * @brief Obtém o identificador da reserva.
 *
 * @param reserva Reserva.
 * @return String com o id (propriedade da reserva; não libertar).
 */
const char* reserva_get_reservation_id(const Reserva *reserva);

// Devolve a string com a lista de flight ids tal como vem no CSV (ex.: "['AB12345', 'CD67890']")
/**
 * @brief Obtém a string com a lista de flight ids.
 *
 * A string é devolvida no formato original do CSV.
 *
 * @param reserva Reserva.
 * @return String com a lista de flight ids (propriedade da reserva; não libertar).
 */
const char* reserva_get_flight_ids(const Reserva *reserva);

// Documento do passageiro associado à reserva
/**
 * @brief Obtém o número de documento do passageiro associado.
 *
 * @param reserva Reserva.
 * @return String com o número de documento (propriedade da reserva; não libertar).
 */
const char* reserva_get_document_number(const Reserva *reserva);

// Preço total da reserva.
/**
 * @brief Obtém o preço total da reserva.
 *
 * @param reserva Reserva.
 * @return Preço total.
 */
double reserva_get_price(const Reserva *reserva);

#endif