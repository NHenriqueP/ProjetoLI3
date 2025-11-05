#include <aeronaves.h>
#include <stdlib.h>
#include <string.h>

typedef struct Aeronave {
    char *id;
    char *manufacturer;
    char *model;
    int ano_fabricação;
    int capacidade;
    int range;
} aeronave;

typedef struct {
    aeronave **aeronaves;
    int count;
    int capacity;
} aeronavePtrManager;

aeronavePtrManager *aeronave_ptr_manager_create() {
    aeronavePtrManager *m = malloc(sizeof(aeronavePtrManager));
    if (!m) return NULL;
    
    m->capacity = 1024;
    m->count = 0;
    
    // 1. Alocação de memória usando malloc
    size_t size_in_bytes = (size_t)m->capacity * sizeof(aeronave*);
    m->aeronaves = malloc(size_in_bytes);
    
    if (!m->aeronaves) { 
        free(m); 
        return NULL; 
    }
    
    // 2. Inicialização manual para zero (ESSENCIAL)
    // Inicializa todos os ponteiros para NULL de forma segura
    for (int i = 0; i < m->capacity; i++) {
        m->aeronaves[i] = NULL;
    }
    
    return m;
}