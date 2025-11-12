#include "gestor_aeroporto.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct AeroportoManager {
    GHashTable *airports_by_iata;  // Hash table para busca por IATA
    GArray *airports_list;         // Array para iteração
    int count;
};

struct aeroporto_ptr_manager {
    GArray *airports; /* array of (Aeroporto*) */
};

static AeroportoManager *mgr = NULL;

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

AeroportoManager* airport_manager_create() {
    AeroportoManager *manager = malloc(sizeof(AeroportoManager));
    if (!manager) return NULL;

    manager->count = 0;
    manager->airports_by_iata = g_hash_table_new(g_str_hash, g_str_equal);
    manager->airports_list = g_array_new(FALSE, FALSE, sizeof(Aeroporto*));

    if (!manager->airports_by_iata || !manager->airports_list) {
        if (manager->airports_by_iata) g_hash_table_destroy(manager->airports_by_iata);
        if (manager->airports_list) g_array_free(manager->airports_list, TRUE);
        free(manager);
        return NULL;
    }

    return manager;
}

void init_airport_manager() {
    if (!mgr) {
        mgr = airport_manager_create();
    }
}

AeroportoManager* get_airport_manager() {
    return mgr;
}

int airport_manager_add(AeroportoManager *manager, Aeroporto airport) {
    if (!manager) return 0;

    Aeroporto *new_airport = g_new(Aeroporto, 1);
    if (!new_airport) return 0;

    *new_airport = airport;

    g_hash_table_insert(manager->airports_by_iata, new_airport->iata, new_airport);
    g_array_append_val(manager->airports_list, new_airport);

    manager->count++;
    return 1;
}

Aeroporto* airport_manager_find_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return NULL;
    return (Aeroporto*)g_hash_table_lookup(manager->airports_by_iata, iata);
}

int airport_exists_by_iata(AeroportoManager *manager, const char *iata) {
    if (!manager || !iata) return 0;
    return g_hash_table_contains(manager->airports_by_iata, iata);
}

int airport_manager_get_count(AeroportoManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

Aeroporto* airport_manager_get_at_index(AeroportoManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->airports_list, Aeroporto*, index);
}

void airport_manager_destroy(AeroportoManager *manager) {
    if (!manager) return;

    for (guint i = 0; i < manager->airports_list->len; i++) {
        Aeroporto *a = g_array_index(manager->airports_list, Aeroporto*, i);
        if (a) {
            free_airport(a);
            g_free(a);
        }
    }

    g_hash_table_destroy(manager->airports_by_iata);
    g_array_free(manager->airports_list, TRUE);

    free(manager);
}

void cleanup_airport_manager() {
    if (mgr) {
        airport_manager_destroy(mgr);
        mgr = NULL;
    }
}

AeroportoPtrManager* airport_ptr_manager_create() {
    AeroportoPtrManager *m = malloc(sizeof(AeroportoPtrManager));
    if (!m) return NULL;

    m->airports = g_array_new(FALSE, FALSE, sizeof(Aeroporto*));
    if (!m->airports) {
        free(m);
        return NULL;
    }

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
