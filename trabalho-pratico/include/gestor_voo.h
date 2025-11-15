#ifndef GESTOR_VOO_H
#define GESTOR_VOO_H

#include <glib.h>
#include "voos.h"

typedef struct voo_ptr_manager VooPtrManager;

VooPtrManager* voo_ptr_manager_create();
void voo_ptr_manager_destroy(VooPtrManager *manager);
int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id);

guint voo_ptr_manager_count(const VooPtrManager *manager);
const Voo* voo_ptr_manager_get(const VooPtrManager *manager, guint index);

#endif
