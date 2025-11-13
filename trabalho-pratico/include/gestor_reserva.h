#ifndef GESTOR_RESERVA_H
#define GESTOR_RESERVA_H

#include <glib.h>
#include "reservas.h"

typedef struct reserva_ptr_manager ReservaPtrManager;

ReservaPtrManager* reserva_ptr_manager_create();
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);
int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade);
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id);
Reserva* reserva_ptr_manager_find_by_document(ReservaPtrManager *manager, const char *document_number);
guint reserva_ptr_manager_count(const ReservaPtrManager *manager);
const Reserva* reserva_ptr_manager_get(const ReservaPtrManager *manager, guint index);

#endif
