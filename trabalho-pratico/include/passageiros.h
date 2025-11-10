#ifndef PASSAGEIRO
#define PASSAGEIRO

#include <stddef.h>
#include <glib.h>

// Estrutura opaca - definição completa no .c
typedef struct passageiro Passageiro;
typedef struct PassageiroManager PassageiroManager;

// Gestor baseado em ponteiros (usa GArray)
typedef struct passageiro_ptr_manager PassageiroPtrManager;

// Criação e destruição do gestor
PassageiroManager* passageiro_manager_create();
void passageiro_manager_destroy(PassageiroManager *manager);

// Adicionar passageiro (cópia profunda)
int passageiro_manager_add(PassageiroManager *manager, Passageiro passageiro);

// Pesquisa
Passageiro* passageiro_manager_find_by_doc(PassageiroManager *manager, const char *document_number);
Passageiro* passageiro_manager_find_by_email(PassageiroManager *manager, const char *email);

// Obter número de passageiros
int passageiro_manager_get_count(PassageiroManager *manager);

// Obter passageiro por índice (para iteração)
Passageiro* passageiro_manager_get_at_index(PassageiroManager *manager, int index);

// Resumo formatado de um passageiro
int passageiro_summary(const Passageiro *passageiro, char *buf, size_t bufsize);

/* Pointer-based manager API */
PassageiroPtrManager* passageiro_ptr_manager_create();
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager);
int passageiro_ptr_manager_add_from_ptr(PassageiroPtrManager *manager, Passageiro *entidade);
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number);

// Adapter compatível com csv_parser genérico
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Getters para Passageiro
const char* passageiro_get_document_number(const Passageiro *p);
const char* passageiro_get_first_name(const Passageiro *p);
const char* passageiro_get_last_name(const Passageiro *p);
const char* passageiro_get_dob(const Passageiro *p);
const char* passageiro_get_nationality(const Passageiro *p);
const char* passageiro_get_gender(const Passageiro *p);
const char* passageiro_get_email(const Passageiro *p);
const char* passageiro_get_phone(const Passageiro *p);
const char* passageiro_get_address(const Passageiro *p);
const char* passageiro_get_photo(const Passageiro *p);

#endif