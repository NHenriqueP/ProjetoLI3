#include <aeroportos.h>
#include <aeroportos.h>
#include <stdlib.h>
#include <string.h>

// Variável global para o manager (inicializada como NULL)
static AeroportoManager *mgr = NULL; // static faz com que seja so visivel neste ficheiro

// Cria um novo AeroportoManager
AeroportoManager* airport_manager_create() {
    AeroportoManager *manager = malloc(sizeof(AeroportoManager));
    if (!manager) return NULL;
    
    manager->capacity = 10000; // capacidade inicial para suportar 10.000 aeroportos
    manager->count = 0;
    manager->airports = malloc(sizeof(Aeroporto) * manager->capacity);
    
    if (!manager->airports) {
        free(manager);
        return NULL;
    }
    
    return manager;
}

// Inicializa o manager global
void init_airport_manager() {
    if (!mgr) {
        mgr = airport_manager_create();
    }
}

// Obtém o manager global
AeroportoManager* get_airport_manager() {
    return mgr;
}

// Expande a capacidade do array se necessário
static int expand_capacity(AeroportoManager *manager) {
    int new_capacity = manager->capacity * 2;
    Aeroporto *new_airports = realloc(manager->airports, sizeof(Aeroporto) * new_capacity);
    
    if (!new_airports) return 0;
    
    manager->airports = new_airports;
    manager->capacity = new_capacity;
    return 1;
}

// Adiciona um aeroporto ao manager
int airport_manager_add(AeroportoManager *manager, Aeroporto airport) {
    if (!manager) return 0;
    
    // Verifica se precisa expandir
    if (manager->count >= manager->capacity) {
        if (!expand_capacity(manager)) return 0;
    }
    
    // Copia o aeroporto para o array
    manager->airports[manager->count] = airport;
    manager->count++;
    
    return 1;
}

// Procura um aeroporto por IATA
Aeroporto* airport_manager_find_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].iata && strcmp(manager->airports[i].iata, iata) == 0) {
            return &manager->airports[i];
        }
    }
    
    return NULL;
}

// Procura um aeroporto por ICAO
Aeroporto* airport_manager_find_by_icao(AeroportoManager *manager, const char *icao) {
    if (!manager || !icao) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].icao && strcmp(manager->airports[i].icao, icao) == 0) {
            return &manager->airports[i];
        }
    }
    
    return NULL;
}

// Obtém o número de aeroportos
int airport_manager_count(AeroportoManager *manager) {
    if (manager) {
        return manager->count;
    }
    return 0;
}

// Liberta a memória de um aeroporto
static void free_airport(Aeroporto *airport) {
    if (!airport) return;
    
    free(airport->iata);
    free(airport->nome);
    free(airport->cidade);
    free(airport->pais);
    free(airport->latitude);
    free(airport->longitude);
    free(airport->icao);
}

// Liberta toda a memória do manager
void airport_manager_destroy(AeroportoManager *manager) {
    if (!manager) return;
    
    // Liberta cada aeroporto
    for (int i = 0; i < manager->count; i++) {
        free_airport(&manager->airports[i]);
    }
    
    free(manager->airports);
    free(manager);
}

// Liberta o manager global
void cleanup_airport_manager() {
    if (mgr) {
        airport_manager_destroy(mgr);
        mgr = NULL;
    }
}

// ========= Funcoes de ler valores =========

// Obtém o código IATA de um aeroporto
const char* airport_get_iata(Aeroporto *airport) {
    if (airport) {
        return airport->iata;
    }
    return NULL;
}

// Obtém o nome de um aeroporto
const char* airport_get_nome(Aeroporto *airport) {
    if (airport) {
        return airport->nome;
    }
    return NULL;
}

// Obtém a cidade de um aeroporto
const char* airport_get_cidade(Aeroporto *airport) {
    if (airport) {
        return airport->cidade;
    }
    return NULL;
}

// Obtém o país de um aeroporto
const char* airport_get_pais(Aeroporto *airport) {
    if (airport) {
        return airport->pais;
    }
    return NULL;
}

// Obtém a latitude de um aeroporto
const char* airport_get_latitude(Aeroporto *airport) {
    if (airport) {
        return airport->latitude;
    }
    return NULL;
}

// Obtém a longitude de um aeroporto
const char* airport_get_longitude(Aeroporto *airport) {
    if (airport) {
        return airport->longitude;
    }
    return NULL;
}

// Obtém o código ICAO de um aeroporto
const char* airport_get_icao(Aeroporto *airport) {
    if (airport) {
        return airport->icao;
    }
    return NULL;
}

// Obtém o tipo de um aeroporto
Tipo airport_get_tipo(Aeroporto *airport) {
    if (airport) {
        return airport->tipo;
    }
    return -1;
}

// ========= Funcoes de escrever valores =========

// Define o código IATA de um aeroporto
int airport_set_iata(Aeroporto *airport, const char *iata) {
    if (!airport || !iata) return 0;
    free(airport->iata);
    airport->iata = strdup(iata);
    return airport->iata != NULL;
}

// Define o nome de um aeroporto
int airport_set_nome(Aeroporto *airport, const char *nome) {
    if (!airport || !nome) return 0;
    free(airport->nome);
    airport->nome = strdup(nome);
    return airport->nome != NULL;
}

// Define a cidade de um aeroporto
int airport_set_cidade(Aeroporto *airport, const char *cidade) {
    if (!airport || !cidade) return 0;
    free(airport->cidade);
    airport->cidade = strdup(cidade);
    return airport->cidade != NULL;
}

// Define o país de um aeroporto
int airport_set_pais(Aeroporto *airport, const char *pais) {
    if (!airport || !pais) return 0;
    free(airport->pais);
    airport->pais = strdup(pais);
    return airport->pais != NULL;
}

// Define a latitude de um aeroporto
int airport_set_latitude(Aeroporto *airport, const char *latitude) {
    if (!airport || !latitude) return 0;
    free(airport->latitude);
    airport->latitude = strdup(latitude);
    return airport->latitude != NULL;
}

// Define a longitude de um aeroporto
int airport_set_longitude(Aeroporto *airport, const char *longitude) {
    if (!airport || !longitude) return 0;
    free(airport->longitude);
    airport->longitude = strdup(longitude);
    return airport->longitude != NULL;
}

// Define o código ICAO de um aeroporto
int airport_set_icao(Aeroporto *airport, const char *icao) {
    if (!airport || !icao) return 0;
    free(airport->icao);
    airport->icao = strdup(icao);
    return airport->icao != NULL;
}

// Define o tipo de um aeroporto
int airport_set_tipo(Aeroporto *airport, Tipo tipo) {
    if (!airport) return 0;
    airport->tipo = tipo;
    return 1;
}

// ==================== FUNÇÕES DE PESQUISA ADICIONAIS ====================

// Procura aeroportos por cidade
Aeroporto** airport_manager_find_by_cidade(AeroportoManager *manager, const char *cidade, int *count) {
    if (!manager || !cidade || !count) return NULL;
    
    *count = 0;
    
    // Conta quantos aeroportos existem nessa cidade
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].cidade && strcmp(manager->airports[i].cidade, cidade) == 0) {
            (*count)++;
        }
    }
    
    if (*count == 0) return NULL;
    
    // Aloca array para os resultados
    Aeroporto **results = malloc(sizeof(Aeroporto*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // Preenche o array com os aeroportos encontrados
    int index = 0;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].cidade && strcmp(manager->airports[i].cidade, cidade) == 0) {
            results[index++] = &manager->airports[i];
        }
    }
    
    return results;
}

// Procura aeroportos por país
Aeroporto** airport_manager_find_by_pais(AeroportoManager *manager, const char *pais, int *count) {
    if (!manager || !pais || !count) return NULL;
    
    *count = 0;
    
    // Conta quantos aeroportos existem nesse país
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].pais && strcmp(manager->airports[i].pais, pais) == 0) {
            (*count)++;
        }
    }
    
    if (*count == 0) return NULL;
    
    // Aloca array para os resultados
    Aeroporto **results = malloc(sizeof(Aeroporto*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // Preenche o array com os aeroportos encontrados
    int index = 0;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].pais && strcmp(manager->airports[i].pais, pais) == 0) {
            results[index++] = &manager->airports[i];
        }
    }
    
    return results;
}

// Procura aeroportos por tipo
Aeroporto** airport_manager_find_by_tipo(AeroportoManager *manager, Tipo tipo, int *count) {
    if (!manager || !count) return NULL;
    
    *count = 0;
    
    // Conta quantos aeroportos existem desse tipo
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].tipo == tipo) {
            (*count)++;
        }
    }
    
    if (*count == 0) return NULL;
    
    // Aloca array para os resultados
    Aeroporto **results = malloc(sizeof(Aeroporto*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // Preenche o array com os aeroportos encontrados
    int index = 0;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].tipo == tipo) {
            results[index++] = &manager->airports[i];
        }
    }
    
    return results;
}

// Procura aeroportos por nome (parcial)
Aeroporto** airport_manager_find_by_nome(AeroportoManager *manager, const char *nome_parcial, int *count) {
    if (!manager || !nome_parcial || !count) return NULL;
    
    *count = 0;
    
    // Conta quantos aeroportos têm esse nome (busca parcial)
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].nome && strstr(manager->airports[i].nome, nome_parcial)) {
            (*count)++;
        }
    }
    
    if (*count == 0) return NULL;
    
    // Aloca array para os resultados
    Aeroporto **results = malloc(sizeof(Aeroporto*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // Preenche o array com os aeroportos encontrados
    int index = 0;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].nome && strstr(manager->airports[i].nome, nome_parcial)) {
            results[index++] = &manager->airports[i];
        }
    }
    
    return results;
}
