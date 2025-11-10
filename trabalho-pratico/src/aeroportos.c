#include "aeroportos.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Variável global para o manager (inicializada como NULL)
static AeroportoManager *mgr = NULL;

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
    GHashTable *airports_by_iata;  // Hash table para busca por IATA
    GArray *airports_list;          // Array para iteração
    int count;
} AeroportoManager;

typedef struct aeroporto_ptr_manager {
    GArray *airports; /* array of (Aeroporto*) */
} AeroportoPtrManager;

// Cria um novo AeroportoManager
AeroportoManager* airport_manager_create() {
    AeroportoManager *manager = malloc(sizeof(AeroportoManager));
    if (!manager) return NULL;
    
    manager->count = 0;
    
    // Cria hash tables com funções de hash e comparação de strings
    manager->airports_by_iata = g_hash_table_new(g_str_hash, g_str_equal);
    manager->airports_list = g_array_new(FALSE, FALSE, sizeof(Aeroporto*));
    
    if (!manager->airports_by_iata  || !manager->airports_list) {
        if (manager->airports_by_iata) g_hash_table_destroy(manager->airports_by_iata);
        if (manager->airports_list) g_array_free(manager->airports_list, TRUE);
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

// Adiciona um aeroporto ao manager
int airport_manager_add(AeroportoManager *manager, Aeroporto airport) {
    if (!manager) return 0;
    
    // Aloca memória para uma cópia do aeroporto
    Aeroporto *new_airport = g_new(Aeroporto, 1);
    if (!new_airport) return 0;
    
    *new_airport = airport;
    
    // Adiciona às hash tables usando IATA como chave
    g_hash_table_insert(manager->airports_by_iata, new_airport->iata, new_airport);
    
    // Adiciona ao array para iteração
    g_array_append_val(manager->airports_list, new_airport);
    
    manager->count++;
    return 1;
}

// Busca um aeroporto por IATA (O(1) em média)
Aeroporto* airport_manager_find_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return NULL;
    return (Aeroporto*)g_hash_table_lookup(manager->airports_by_iata, iata);
}

// Verifica se um aeroporto existe por IATA
int airport_exists_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return 0;
    return g_hash_table_contains(manager->airports_by_iata, iata);
}


// Obtém o número total de aeroportos
int airport_manager_get_count(AeroportoManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

// Obtém um aeroporto por índice (para iteração)
Aeroporto* airport_manager_get_at_index(AeroportoManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->airports_list, Aeroporto*, index);
}

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
    
    // Liberta cada aeroporto do array
    for (guint i = 0; i < manager->airports_list->len; i++) {
        Aeroporto *a = g_array_index(manager->airports_list, Aeroporto*, i);
        if (a) {
            free_airport(a);
            g_free(a);
        }
    }
    
    // Liberta as hash tables (não precisa liberar os valores, já foram liberados acima)
    g_hash_table_destroy(manager->airports_by_iata);
    g_array_free(manager->airports_list, TRUE);
    
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

Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata) {
    if (!manager || !manager->airports || !iata) return NULL;
    for (guint i = 0; i < manager->airports->len; i++) {
        Aeroporto *a = g_array_index(manager->airports, Aeroporto*, i);
        if (a && a->iata && strcmp(a->iata, iata) == 0) return a;
    }
    return NULL;
}

// Produz um resumo CSV do aeroporto (IATA,Nome,Cidade,País,Tipo)
int airport_summary(const Aeroporto *airport, char *buf, size_t bufsize) {
    if (!airport || !buf || bufsize == 0) return 0;
    int n = snprintf(buf, bufsize, "%s,%s,%s,%s,%s",
                     airport->iata ? airport->iata : "",
                     airport->nome ? airport->nome : "",
                     airport->cidade ? airport->cidade : "",
                     airport->pais ? airport->pais : "",
                     airport->tipo ? airport->tipo : "");
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

/* ---------------- CSV airport line parser (movido do csv_parser.c) -------- */
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