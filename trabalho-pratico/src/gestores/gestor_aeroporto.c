#include "gestor_aeroporto.h"

#include <glib.h>
#include <stdlib.h>

struct aeroporto_ptr_manager {
    GHashTable *by_iata; /* key: IATA (owned by Aeroporto), value: Aeroporto* */
};

AeroportoPtrManager* airport_ptr_manager_create() {
    AeroportoPtrManager *m = malloc(sizeof(AeroportoPtrManager));
    if (!m) return NULL;

    m->by_iata = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->by_iata) {
        free(m);
        return NULL;
    }

    return m;
}

void airport_ptr_manager_destroy(AeroportoPtrManager *manager) {
    if (!manager) return;

    if (manager->by_iata) {
        // Destruir todos os aeroportos ao destruir a hash table
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, manager->by_iata);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            Aeroporto *a = (Aeroporto*)value;
            if (a) aeroporto_destroy(a);
        }
        g_hash_table_destroy(manager->by_iata);
    }

    free(manager);
}

int airport_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    AeroportoPtrManager *manager = (AeroportoPtrManager*)gestor;
    Aeroporto *airport = (Aeroporto*)entidade;
    const char *iata = aeroporto_get_iata(airport);
    if (manager->by_iata && g_hash_table_lookup(manager->by_iata, iata)) {
        aeroporto_destroy(airport);
        return 1;
    }
    g_hash_table_insert(manager->by_iata, (gpointer)iata, airport);
    return 1;
}

Aeroporto* airport_ptr_manager_find_by_iata(AeroportoPtrManager *manager, const char *iata) {
    if (!manager || !manager->by_iata || !iata) return NULL;
    return g_hash_table_lookup(manager->by_iata, iata);
}
