#ifndef GESTOR_PASSAGEIRO_H
#define GESTOR_PASSAGEIRO_H

#include <glib.h>
#include "passageiros.h"

typedef struct passageiro_ptr_manager PassageiroPtrManager;

PassageiroPtrManager* passageiro_ptr_manager_create();
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager);
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
int existe_document_number(PassageiroPtrManager *manager, const char *document_number);

// Procura passageiro por document_number (retorna NULL se não existir)
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number);

// Total de passageiros carregados
guint passageiro_ptr_manager_count(const PassageiroPtrManager *manager);

// Acesso indexado aos passageiros (retorna NULL se index for inválido)
const Passageiro* passageiro_ptr_manager_get(const PassageiroPtrManager *manager, guint index);

#endif