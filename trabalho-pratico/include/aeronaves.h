#ifndef AERONAVE
#define AERONAVE

#include <stddef.h>
#include <glib.h>

// Estrutura opaca - definição completa no .c
typedef struct aeronave Aeronave;
typedef struct AeronaveManager AeronaveManager;

// Gestor baseado em ponteiros (usa GArray)
typedef struct {
    GArray *aeronaves; // array de (Aeronave*)
} AeronavePtrManager;

// Criação e destruição do gestor
AeronaveManager* aeronave_manager_create();
void aeronave_manager_destroy(AeronaveManager *manager);

// Adicionar aeronave (cópia profunda)
int aeronave_manager_add(AeronaveManager *manager, Aeronave aeronave);

// Pesquisa
Aeronave* aeronave_manager_find_by_id(AeronaveManager *manager, const char *id);
Aeronave* aeronave_manager_find_by_manufacturer(AeronaveManager *manager, const char *manufacturer);

// Obter número de aeronaves
int aeronave_manager_get_count(AeronaveManager *manager);

// Obter aeronave por índice (para iteração)
Aeronave* aeronave_manager_get_at_index(AeronaveManager *manager, int index);

// Resumo formatado de uma aeronave
int aeronave_summary(const Aeronave *aeronave, char *buf, size_t bufsize);

/* Pointer-based manager API */
AeronavePtrManager* aeronave_ptr_manager_create();
void aeronave_ptr_manager_destroy(AeronavePtrManager *manager);
int aeronave_ptr_manager_add_from_ptr(AeronavePtrManager *manager, Aeronave *entidade);
Aeronave* aeronave_ptr_manager_find_by_id(AeronavePtrManager *manager, const char *id);

// Adapter compatível com csv_parser genérico
int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Getters para Aeronave
const char* aeronave_get_id(const Aeronave *a);
const char* aeronave_get_fabricante(const Aeronave *a);
const char* aeronave_get_modelo(const Aeronave *a);
int aeronave_get_ano(const Aeronave *a);
int aeronave_get_capacidade(const Aeronave *a);
int aeronave_get_alcance(const Aeronave *a);

#endif