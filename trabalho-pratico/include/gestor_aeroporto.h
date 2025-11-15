#ifndef GESTOR_AEROPORTO_H
#define GESTOR_AEROPORTO_H

#include <glib.h>
#include "aeroportos.h"

typedef struct aeroporto_ptr_manager AeroportoPtrManager;

AeroportoPtrManager* airport_ptr_manager_create();
void airport_ptr_manager_destroy(AeroportoPtrManager *manager);
int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata);

#endif
