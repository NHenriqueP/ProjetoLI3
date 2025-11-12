#ifndef GESTOR_RESERVA_H
#define GESTOR_RESERVA_H

#include "reservas.h"

typedef struct ReservaManager ReservaManager;
typedef struct reserva_ptr_manager ReservaPtrManager;

ReservaManager* reserva_manager_create();
void reserva_manager_destroy(ReservaManager *manager);
void init_reserva_manager();
ReservaManager* get_reserva_manager();
int reserva_manager_add(ReservaManager *manager, Reserva reserva);
Reserva* reserva_manager_find_by_id(ReservaManager *manager, const char *reservation_id);
Reserva* reserva_manager_find_by_document(ReservaManager *manager, const char *document_number);
int reserva_exists_by_id(ReservaManager *manager, const char *reservation_id);
int reserva_manager_get_count(ReservaManager *manager);
Reserva* reserva_manager_get_at_index(ReservaManager *manager, int index);
void cleanup_reserva_manager();

ReservaPtrManager* reserva_ptr_manager_create();
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);
int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade);
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id);

#endif
