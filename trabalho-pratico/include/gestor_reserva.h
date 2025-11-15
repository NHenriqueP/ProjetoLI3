#ifndef GESTOR_RESERVA_H
#define GESTOR_RESERVA_H

#include <glib.h>
#include "reservas.h"

typedef struct reserva_ptr_manager ReservaPtrManager;

ReservaPtrManager* reserva_ptr_manager_create();
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

#endif
