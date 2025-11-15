#include "gestor_aeronave.h"

#include <glib.h>
#include <stdlib.h>

struct aeronave_ptr_manager {
    GArray *aeronaves; /* array de (Aeronave*) */
    GHashTable *by_id;
};

static void free_aeronave(Aeronave *aeronave) {
    if (!aeronave) return;

    g_free(aeronave->id);
    g_free(aeronave->fabricante);
    g_free(aeronave->modelo);
}
AeronavePtrManager* aeronave_ptr_manager_create() {
    AeronavePtrManager *m = malloc(sizeof(AeronavePtrManager));
    if (!m) return NULL;

    m->aeronaves = g_array_new(FALSE, FALSE, sizeof(Aeronave*));
    m->by_id = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->aeronaves || !m->by_id) {
        if (m->aeronaves) g_array_free(m->aeronaves, TRUE);
        if (m->by_id) g_hash_table_destroy(m->by_id);
        free(m);
        return NULL;
    }

    return m;
}

void aeronave_ptr_manager_destroy(AeronavePtrManager *manager) {
    if (!manager) return;

    if (manager->aeronaves) {
        for (guint i = 0; i < manager->aeronaves->len; i++) {
            Aeronave *a = g_array_index(manager->aeronaves, Aeronave*, i);
            if (a) {
                free_aeronave(a);
                g_free(a);
            }
        }
        g_array_free(manager->aeronaves, TRUE);
    }

    if (manager->by_id) g_hash_table_destroy(manager->by_id);

    free(manager);
}

int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    AeronavePtrManager *manager = (AeronavePtrManager*)gestor;
    Aeronave *aeronave = (Aeronave*)entidade;
    if (manager->by_id && g_hash_table_lookup(manager->by_id, aeronave->id)) {
        free_aeronave(aeronave);
        g_free(aeronave);
        return 1;
    }
    g_array_append_val(manager->aeronaves, aeronave);
    g_hash_table_insert(manager->by_id, aeronave->id, aeronave);
    return 1;
}

int existe_aeronave(AeronavePtrManager *manager, const char *id) {
    if (!manager || !id) return 0;
    return g_hash_table_contains(manager->by_id, id);
}

guint aeronave_ptr_manager_count(const AeronavePtrManager *manager) {
    if (!manager || !manager->aeronaves) return 0;
    return manager->aeronaves->len;
}

const Aeronave* aeronave_ptr_manager_get(const AeronavePtrManager *manager, guint index) {
    if (!manager || !manager->aeronaves || index >= manager->aeronaves->len) return NULL;
    return g_array_index(manager->aeronaves, Aeronave*, index);
}
