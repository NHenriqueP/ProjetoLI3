#ifndef GESTOR_RESERVA_H
#define GESTOR_RESERVA_H

#include <glib.h>
#include "reservas.h"

typedef struct reserva_ptr_manager ReservaPtrManager;

ReservaPtrManager* reserva_ptr_manager_create();
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Total de reservas carregadas
guint reserva_ptr_manager_count(const ReservaPtrManager *manager);

// Acesso indexado às reservas (retorna NULL se index for inválido)
const Reserva* reserva_ptr_manager_get(const ReservaPtrManager *manager, guint index);

// Procura por id da reserva (retorna NULL se não existir)
const Reserva* reserva_ptr_manager_find_by_id(const ReservaPtrManager *manager, const char *id);

#endif