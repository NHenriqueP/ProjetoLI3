#include "gestor_aeroporto.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct aeroporto_ptr_manager {
    GArray *airports; /* array of (Aeroporto*) */
    GHashTable *by_iata;
};

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

AeroportoPtrManager* airport_ptr_manager_create() {
    AeroportoPtrManager *m = malloc(sizeof(AeroportoPtrManager));
    if (!m) return NULL;

    m->airports = g_array_new(FALSE, FALSE, sizeof(Aeroporto*));
    m->by_iata = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->airports || !m->by_iata) {
        if (m->airports) g_array_free(m->airports, TRUE);
        if (m->by_iata) g_hash_table_destroy(m->by_iata);
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

    if (manager->by_iata) g_hash_table_destroy(manager->by_iata);

    free(manager);
}

int airport_ptr_manager_add_from_ptr(AeroportoPtrManager *manager, Aeroporto *entidade) {
    if (!manager || !entidade) return 0;
    if (manager->by_iata && g_hash_table_lookup(manager->by_iata, entidade->iata)) {
        free_airport(entidade);
        g_free(entidade);
        return 1;
    }
    g_array_append_val(manager->airports, entidade);
    g_hash_table_insert(manager->by_iata, entidade->iata, entidade);
    return 1;
}

int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return airport_ptr_manager_add_from_ptr((AeroportoPtrManager*)gestor, (Aeroporto*)entidade);
}

guint airport_ptr_manager_count(const AeroportoPtrManager *manager) {
    if (!manager || !manager->airports) return 0;
    return manager->airports->len;
}

const Aeroporto* airport_ptr_manager_get(const AeroportoPtrManager *manager, guint index) {
    if (!manager || !manager->airports || index >= manager->airports->len) return NULL;
    return g_array_index(manager->airports, Aeroporto*, index);
}

Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata) {
    if (!manager || !manager->by_iata || !iata) return NULL;
    return g_hash_table_lookup(manager->by_iata, iata);
}
