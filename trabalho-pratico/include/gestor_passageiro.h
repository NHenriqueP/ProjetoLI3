#ifndef GESTOR_PASSAGEIRO_H
#define GESTOR_PASSAGEIRO_H

#include <glib.h>
#include "passageiros.h"

typedef struct passageiro_ptr_manager PassageiroPtrManager;

PassageiroPtrManager* passageiro_ptr_manager_create();
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager);
int passageiro_ptr_manager_add_from_ptr(PassageiroPtrManager *manager, Passageiro *entidade);
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number);
guint passageiro_ptr_manager_count(const PassageiroPtrManager *manager);
const Passageiro* passageiro_ptr_manager_get(const PassageiroPtrManager *manager, guint index);

#endif
