#include "gestor_voo.h"

#include <glib.h>
#include <stdlib.h>

struct voo_ptr_manager {
    GArray *flights;
    GHashTable *by_id;
};

VooPtrManager* voo_ptr_manager_create() {
    VooPtrManager *m = malloc(sizeof(VooPtrManager));
    if (!m) return NULL;

    m->flights = g_array_new(FALSE, FALSE, sizeof(Voo*));
    m->by_id = g_hash_table_new(g_str_hash, g_str_equal);

    if (!m->flights || !m->by_id) {
        if (m->flights) g_array_free(m->flights, TRUE);
        if (m->by_id) g_hash_table_destroy(m->by_id);
        free(m);
        return NULL;
    }

    return m;
}

void voo_ptr_manager_destroy(VooPtrManager *manager) {
    if (!manager) return;

    if (manager->flights) {
        for (guint i = 0; i < manager->flights->len; i++) {
            Voo *v = g_array_index(manager->flights, Voo*, i);
            if (v) voo_destroy(v);
        }
        g_array_free(manager->flights, TRUE);
    }

    if (manager->by_id) g_hash_table_destroy(manager->by_id);
    free(manager);
}

int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    VooPtrManager *manager = (VooPtrManager*)gestor;
    Voo *voo = (Voo*)entidade;
    const char *id = voo_get_id(voo);

    if (manager->by_id && g_hash_table_lookup(manager->by_id, id)) {
        voo_destroy(voo);
        return 1;
    }

    g_array_append_val(manager->flights, voo);
    g_hash_table_insert(manager->by_id, (gpointer)id, voo);
    return 1;
}

Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id) {
    if (!manager || !manager->by_id || !id) return NULL;
    return g_hash_table_lookup(manager->by_id, id);
}

guint voo_ptr_manager_count(const VooPtrManager *manager) {
    if (!manager || !manager->flights) return 0;
    return manager->flights->len;
}

const Voo* voo_ptr_manager_get(const VooPtrManager *manager, guint index) {
    if (!manager || !manager->flights || index >= manager->flights->len) return NULL;
    return g_array_index(manager->flights, Voo*, index);
}
