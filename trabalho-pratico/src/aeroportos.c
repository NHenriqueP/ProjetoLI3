#include "aeroportos.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

// Variável global para o manager (inicializada como NULL)
static AeroportoManager *mgr = NULL; // static faz com que seja so visivel neste ficheiro

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

typedef struct AeroportoManager{
    Aeroporto *airports;
    int count;
    int capacity;
} AeroportoManager;

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


// Liberta a memória de um aeroporto
static void free_airport(Aeroporto *airport) {
    if (!airport) return;

    g_free(airport->iata);
    g_free(airport->nome);
    g_free(airport->cidade);
    g_free(airport->pais);
    g_free(airport->latitude);
    g_free(airport->longitude);
    g_free(airport->icao);
    g_free(airport->tipo);
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

/* ---------------- Pointer-based manager using GArray (implementations) -------- */

// Create pointer-based manager
AeroportoPtrManager* airport_ptr_manager_create() {
    AeroportoPtrManager *m = malloc(sizeof(AeroportoPtrManager));
    if (!m) return NULL;
    m->airports = g_array_new(FALSE, FALSE, sizeof(Aeroporto*));
    if (!m->airports) { free(m); return NULL; }
    return m;
}

void airport_ptr_manager_destroy(AeroportoPtrManager *manager) {
    if (!manager) return;
    if (manager->airports) {
        for (guint i = 0; i < manager->airports->len; i++) {
            Aeroporto *a = g_array_index(manager->airports, Aeroporto*, i);
            if (a) {
                free_airport(a);
                g_free(a);
            }
        }
        g_array_free(manager->airports, TRUE);
    }
    free(manager);
}

int airport_ptr_manager_add_from_ptr(AeroportoPtrManager *manager, Aeroporto *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->airports, entidade);
    return 1;
}

int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return airport_ptr_manager_add_from_ptr((AeroportoPtrManager*)gestor, (Aeroporto*)entidade);
}


/* ---------------- CSV airport line parser (moved from csv_parser.c) -------- */
ParseResultado parse_airport_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    r.motivo = NULL;

    // Esperamos pelo menos 8 campos: IATA, Nome, Cidade, País, Latitude, Longitude, ICAO, Tipo
    if (n_fields < 8) {
        r.motivo = "Número incorreto de campos";
        return r;
    }

    char *iata      = fields[0];
    char *nome      = fields[1];
    char *cidade    = fields[2];
    char *pais      = fields[3];
    char *latitude  = fields[4];
    char *longitude = fields[5];
    char *icao      = fields[6];
    char *tipo      = fields[7];

    if (!validar_iata(iata)) {
        r.motivo = "IATA inválido";
        return r;
    }

    if (!validar_icao(icao)) {
        r.motivo = "ICAO inválido";
        return r;
    }

    if (!validar_latitude(latitude) || !validar_longitude(longitude)) {
        r.motivo = "Latitude ou longitude inválida";
        return r;
    }

    if (!nome || !cidade || !pais || strlen(nome) == 0 || strlen(cidade) == 0 || strlen(pais) == 0) {
        r.motivo = "Campos de texto vazios";
        return r;
    }

    if (tipo_aeroporto_valido(tipo) == 0) {
        r.motivo = "tipo do aeroporto inválido";
        return r;
    }

    Aeroporto *a = g_new0(Aeroporto, 1);
    if (!a) {
        r.motivo = "Falha de memória ao criar aeroporto";
        return r;
    }

    a->iata      = g_strdup(iata);
    a->nome      = g_strdup(nome);
    a->cidade    = g_strdup(cidade);
    a->pais      = g_strdup(pais);
    a->latitude  = g_strdup(latitude);
    a->longitude = g_strdup(longitude);
    a->icao      = g_strdup(icao);
    a->tipo      = g_strdup(tipo);

    if (!a->iata || !a->nome || !a->cidade || !a->pais ||
        !a->latitude || !a->longitude || !a->icao || !a->tipo) {
        g_free(a->iata); g_free(a->nome); g_free(a->cidade);
        g_free(a->pais); g_free(a->latitude); g_free(a->longitude);
        g_free(a->icao); g_free(a->tipo); g_free(a);
        r.motivo = "Erro de alocação de memória";
        return r;
    }

    r.valido = true;
    r.entidade = a;
    return r;
}




