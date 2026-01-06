#ifndef GESTOR_RESERVA_H
#define GESTOR_RESERVA_H

#include <glib.h>
#include "reservas.h"

typedef struct reserva_ptr_manager ReservaPtrManager;

/**
 * @file gestor_reserva.h
 * @brief Gestor de reservas (armazenamento e pesquisa).
 */

/**
 * @brief Cria um gestor de reservas.
 *
 * @return Ponteiro para o gestor criado, ou `NULL` em caso de falha.
 */
ReservaPtrManager* reserva_ptr_manager_create();

/**
 * @brief Destrói um gestor de reservas.
 *
 * @param manager Gestor a destruir.
 * @return Nada (função `void`).
 */
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);

/**
 * @brief Adapter de inserção usado pelo parser genérico.
 *
 * @param gestor Ponteiro genérico para `ReservaPtrManager`.
 * @param entidade Ponteiro para a entidade a inserir (tipicamente `Reserva*`).
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Total de reservas carregadas
/**
 * @brief Número total de reservas carregadas.
 *
 * @param manager Gestor de reservas.
 * @return Número de reservas.
 */
guint reserva_ptr_manager_count(const ReservaPtrManager *manager);

// Acesso indexado às reservas (retorna NULL se index for inválido)
/**
 * @brief Acesso indexado a uma reserva.
 *
 * @param manager Gestor de reservas.
 * @param index Índice (0..count-1).
 * @return Ponteiro para a reserva no índice, ou `NULL` se o índice for inválido.
 */
const Reserva* reserva_ptr_manager_get(const ReservaPtrManager *manager, guint index);

#endif