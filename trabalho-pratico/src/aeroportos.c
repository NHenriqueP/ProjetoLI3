#include <aeroportos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// Forward declaration para uso em copy_airport
static void free_airport(Aeroporto *airport);

// Cópia profunda dos campos de Aeroporto
static int copy_airport(Aeroporto *dst, const Aeroporto *src) {
    memset(dst, 0, sizeof(*dst));
    if (src->iata)      { dst->iata = strdup(src->iata); if (!dst->iata) goto fail; }
    if (src->nome)      { dst->nome = strdup(src->nome); if (!dst->nome) goto fail; }
    if (src->cidade)    { dst->cidade = strdup(src->cidade); if (!dst->cidade) goto fail; }
    if (src->pais)      { dst->pais = strdup(src->pais); if (!dst->pais) goto fail; }
    if (src->latitude)  { dst->latitude = strdup(src->latitude); if (!dst->latitude) goto fail; }
    if (src->longitude) { dst->longitude = strdup(src->longitude); if (!dst->longitude) goto fail; }
    if (src->icao)      { dst->icao = strdup(src->icao); if (!dst->icao) goto fail; }
    if (src->tipo)      { dst->tipo = strdup(src->tipo); if (!dst->tipo) goto fail; }
    return 1;
fail:
    free_airport(dst);
    return 0;
}

// Função adaptadora para o CSV parser: recebe void* e const void* e adiciona ao gestor
int airport_manager_add_from_ptr(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    AeroportoManager *manager = (AeroportoManager*)gestor;
    const Aeroporto *src = (const Aeroporto*)entidade;

    // Expandir se necessário
    if (manager->count >= manager->capacity) {
        if (!expand_capacity(manager)) return 0;
    }

    Aeroporto *slot = &manager->airports[manager->count];
    if (!copy_airport(slot, src)) {
        return 0;
    }
    manager->count++;

    // Transferência de propriedade: libertar a entidade original
    // (assumimos que foi alocada dinamicamente pelo parser)
    Aeroporto *to_free = (Aeroporto*)entidade;
    free_airport(to_free);
    free(to_free);

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

// Procura por ICAO
Aeroporto* airport_manager_find_by_icao(AeroportoManager *manager, const char *icao) {
    if (!manager || !icao) return NULL;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].icao && strcmp(manager->airports[i].icao, icao) == 0) {
            return &manager->airports[i];
        }
    }
    return NULL;
}

// Procura por código (IATA de 3 chars ou ICAO de 4 chars)
Aeroporto* airport_manager_find_by_code(AeroportoManager *manager, const char *code) {
    if (!manager || !code) return NULL;
    size_t len = strlen(code);
    if (len == 3) return airport_manager_find_by_iata(manager, code);
    if (len == 4) return airport_manager_find_by_icao(manager, code);
    // fallback: tenta ambos
    Aeroporto *a = airport_manager_find_by_iata(manager, code);
    if (a) return a;
    return airport_manager_find_by_icao(manager, code);
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
    free(airport->tipo);
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


// Obtém o código IATA de um aeroporto
const char* airport_get_iata(Aeroporto *airport) {
    if (airport) {
        return airport->iata;
    }
    return NULL;
}



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
int airport_set_tipo(Aeroporto *airport, char *tipo) {
    if (!airport) return 0;
    airport->tipo = tipo;
    return 1;
}

// Devolve o índice do aeroporto com o IATA dado, ou -1 se não existir
int airport_manager_index_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return -1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->airports[i].iata && strcmp(manager->airports[i].iata, iata) == 0) {
            return i;
        }
    }
    return -1;
}

// Escreve um resumo textual do aeroporto no buffer fornecido
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize) {
    if (!airport || !buf || bufsize == 0) return 0;
    const char *iata = airport->iata ? airport->iata : "";
    const char *icao = airport->icao ? airport->icao : "";
    const char *nome = airport->nome ? airport->nome : "";
    const char *cidade = airport->cidade ? airport->cidade : "";
    const char *pais = airport->pais ? airport->pais : "";
    const char *lat = airport->latitude ? airport->latitude : "";
    const char *lon = airport->longitude ? airport->longitude : "";
    const char *tipo = airport->tipo ? airport->tipo : "";

    int written = snprintf(
        buf, bufsize,
        "IATA: %s\nICAO: %s\nNome: %s\nCidade: %s\nPaís: %s\nLatitude: %s\nLongitude: %s\nTipo: %s",
        iata, icao, nome, cidade, pais, lat, lon, tipo
    );
    return (written >= 0 && (size_t)written < bufsize) ? 1 : 0;
}

