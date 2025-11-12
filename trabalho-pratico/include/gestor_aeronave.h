#ifndef GESTOR_AERONAVE_H
#define GESTOR_AERONAVE_H

#include <glib.h>

#include "aeronaves.h"

typedef struct AeronaveManager AeronaveManager;
typedef struct aeronave_ptr_manager AeronavePtrManager;

AeronaveManager* aeronave_manager_create();
void aeronave_manager_destroy(AeronaveManager *manager);
void init_aeronave_manager();
AeronaveManager* get_aeronave_manager();
int aeronave_manager_add(AeronaveManager *manager, Aeronave aeronave);
Aeronave* aeronave_manager_find_by_id(AeronaveManager *manager, const char *id);
Aeronave* aeronave_manager_find_by_manufacturer(AeronaveManager *manager, const char *manufacturer);
int aeronave_exists_by_id(AeronaveManager *manager, const char *id);
int aeronave_manager_get_count(AeronaveManager *manager);
Aeronave* aeronave_manager_get_at_index(AeronaveManager *manager, int index);
void cleanup_aeronave_manager();

AeronavePtrManager* aeronave_ptr_manager_create();
void aeronave_ptr_manager_destroy(AeronavePtrManager *manager);
int aeronave_ptr_manager_add_from_ptr(AeronavePtrManager *manager, Aeronave *entidade);
int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Aeronave* aeronave_ptr_manager_find_by_id(AeronavePtrManager *manager, const char *id);
guint aeronave_ptr_manager_count(const AeronavePtrManager *manager);
const Aeronave* aeronave_ptr_manager_get(const AeronavePtrManager *manager, guint index);

#endif
