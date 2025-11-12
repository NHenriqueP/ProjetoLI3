#ifndef GESTOR_PASSAGEIRO_H
#define GESTOR_PASSAGEIRO_H

#include "passageiros.h"

typedef struct PassageiroManager PassageiroManager;
typedef struct passageiro_ptr_manager PassageiroPtrManager;

PassageiroManager* passageiro_manager_create();
void passageiro_manager_destroy(PassageiroManager *manager);
void init_passageiro_manager();
PassageiroManager* get_passageiro_manager();
int passageiro_manager_add(PassageiroManager *manager, Passageiro passageiro);
Passageiro* passageiro_manager_find_by_doc(PassageiroManager *manager, const char *document_number);
Passageiro* passageiro_manager_find_by_email(PassageiroManager *manager, const char *email);
int passageiro_exists_by_doc(PassageiroManager *manager, const char *document_number);
int passageiro_manager_get_count(PassageiroManager *manager);
Passageiro* passageiro_manager_get_at_index(PassageiroManager *manager, int index);
void cleanup_passageiro_manager();

PassageiroPtrManager* passageiro_ptr_manager_create();
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager);
int passageiro_ptr_manager_add_from_ptr(PassageiroPtrManager *manager, Passageiro *entidade);
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number);

#endif
