#ifndef GESTOR_AERONAVE_H
#define GESTOR_AERONAVE_H

#include <glib.h>
#include "aeronaves.h"

typedef struct aeronave_ptr_manager AeronavePtrManager;

AeronavePtrManager* aeronave_ptr_manager_create();
void aeronave_ptr_manager_destroy(AeronavePtrManager *manager);
int aeronave_ptr_manager_add_from_ptr(AeronavePtrManager *manager, Aeronave *entidade);
int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Aeronave* aeronave_ptr_manager_find_by_id(AeronavePtrManager *manager, const char *id);
guint aeronave_ptr_manager_count(const AeronavePtrManager *manager);
const Aeronave* aeronave_ptr_manager_get(const AeronavePtrManager *manager, guint index);
int existe_aeronave(AeronavePtrManager *manager, const char *id);

#endif
