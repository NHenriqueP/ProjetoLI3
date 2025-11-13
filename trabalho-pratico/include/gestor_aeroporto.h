#ifndef GESTOR_AEROPORTO_H
#define GESTOR_AEROPORTO_H

#include "aeroportos.h"

typedef struct AeroportoManager AeroportoManager;
typedef struct aeroporto_ptr_manager AeroportoPtrManager;

AeroportoManager* airport_manager_create();
void airport_manager_destroy(AeroportoManager *manager);
int airport_manager_add(AeroportoManager *manager, Aeroporto airport);
Aeroporto* airport_manager_find_by_iata(AeroportoManager *manager, const char *iata);
int airport_exists_by_iata(AeroportoManager *manager, const char *iata);
int airport_manager_get_count(AeroportoManager *manager);
Aeroporto* airport_manager_get_at_index(AeroportoManager *manager, int index);

AeroportoPtrManager* airport_ptr_manager_create();
void airport_ptr_manager_destroy(AeroportoPtrManager *manager);
int airport_ptr_manager_add_from_ptr(AeroportoPtrManager *manager, Aeroporto *entidade);
int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata);

#endif
