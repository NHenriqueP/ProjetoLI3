#ifndef AEROPORTO
#define AEROPORTO

#include <stddef.h> // size_t


typedef struct aeroporto {
    char *iata;
    char *nome;
    char *cidade;
    char *pais;
    char *latitude;
    char *longitude;
    char *icao;
    char *tipo;
} Aeroporto;

typedef struct {
    Aeroporto *airports;
    int count;
    int capacity;
} AeroportoManager;


// Criação e destruição do gestor
AeroportoManager* airport_manager_create();
void airport_manager_destroy(AeroportoManager *manager);

// Função compatível com GestorAddFunc (void*, const void*) para o CSV parser
// Recebe um ponteiro para Aeroporto (entidade) e adiciona ao gestor (cópia profunda)
int airport_manager_add_from_ptr(void *gestor, const void *entidade);

// Pesquisa
Aeroporto* airport_manager_find_by_iata(AeroportoManager *manager, const char *iata);
Aeroporto* airport_manager_find_by_icao(AeroportoManager *manager, const char *icao);
Aeroporto* airport_manager_find_by_code(AeroportoManager *manager, const char *code);

// Índice pelo IATA (ou -1 se não existir)
int airport_manager_index_by_iata(AeroportoManager *manager, const char *iata);

// Resumo formatado de um aeroporto no buffer fornecido
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize);






#endif 
