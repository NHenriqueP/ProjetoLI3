#include "gestor_aeronave.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct AeronaveManager {
    GHashTable *aeronaves_by_id;
    GArray *aeronaves_list;
    int count;
};

struct aeronave_ptr_manager {
    GArray *aeronaves; /* array de (Aeronave*) */
};


static void free_aeronave(Aeronave *aeronave) {
    if (!aeronave) return;

    g_free(aeronave->id);
    g_free(aeronave->fabricante);
    g_free(aeronave->modelo);
}

AeronaveManager* aeronave_manager_create() {
    AeronaveManager *manager = malloc(sizeof(AeronaveManager));
    if (!manager) return NULL;

    manager->count = 0;
    manager->aeronaves_by_id = g_hash_table_new(g_str_hash, g_str_equal);
    manager->aeronaves_list = g_array_new(FALSE, FALSE, sizeof(Aeronave*));

    if (!manager->aeronaves_by_id || !manager->aeronaves_list) {
        if (manager->aeronaves_by_id) g_hash_table_destroy(manager->aeronaves_by_id);
        if (manager->aeronaves_list) g_array_free(manager->aeronaves_list, TRUE);
        free(manager);
        return NULL;
    }

    return manager;
}


int aeronave_manager_add(AeronaveManager *manager, Aeronave aeronave) {
    if (!manager) return 0;

    Aeronave *new_aeronave = g_new(Aeronave, 1);
    if (!new_aeronave) return 0;

    *new_aeronave = aeronave;

    g_hash_table_insert(manager->aeronaves_by_id, new_aeronave->id, new_aeronave);
    g_array_append_val(manager->aeronaves_list, new_aeronave);

    manager->count++;
    return 1;
}

Aeronave* aeronave_manager_find_by_id(AeronaveManager *manager, const char *id) {
    if (!manager || !id) return NULL;
    return (Aeronave*)g_hash_table_lookup(manager->aeronaves_by_id, id);
}

Aeronave* aeronave_manager_find_by_manufacturer(AeronaveManager *manager, const char *manufacturer) {
    if (!manager || !manager->aeronaves_list || !manufacturer) return NULL;

    for (guint i = 0; i < manager->aeronaves_list->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves_list, Aeronave*, i);
        if (a && a->fabricante && strcmp(a->fabricante, manufacturer) == 0) {
            return a;
        }
    }
    return NULL;
}

int aeronave_exists_by_id(AeronaveManager *manager, const char *id) {
    if (!manager || !id) return 0;
    return g_hash_table_contains(manager->aeronaves_by_id, id);
}

int aeronave_manager_get_count(AeronaveManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

Aeronave* aeronave_manager_get_at_index(AeronaveManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->aeronaves_list, Aeronave*, index);
}

void aeronave_manager_destroy(AeronaveManager *manager) {
    if (!manager) return;

    for (guint i = 0; i < manager->aeronaves_list->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves_list, Aeronave*, i);
        if (a) {
            free_aeronave(a);
            g_free(a);
        }
    }

    g_hash_table_destroy(manager->aeronaves_by_id);
    g_array_free(manager->aeronaves_list, TRUE);

    free(manager);
}


AeronavePtrManager* aeronave_ptr_manager_create() {
    AeronavePtrManager *m = malloc(sizeof(AeronavePtrManager));
    if (!m) return NULL;

    m->aeronaves = g_array_new(FALSE, FALSE, sizeof(Aeronave*));
    if (!m->aeronaves) {
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

    free(manager);
}

int aeronave_ptr_manager_add_from_ptr(AeronavePtrManager *manager, Aeronave *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->aeronaves, entidade);
    return 1;
}

int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return aeronave_ptr_manager_add_from_ptr((AeronavePtrManager*)gestor, (Aeronave*)entidade);
}

Aeronave* aeronave_ptr_manager_find_by_id(AeronavePtrManager *manager, const char *id) {
    if (!manager || !manager->aeronaves || !id) return NULL;

    for (guint i = 0; i < manager->aeronaves->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves, Aeronave*, i);
        if (a && a->id && strcmp(a->id, id) == 0) return a;
    }

    return NULL;
}

guint aeronave_ptr_manager_count(const AeronavePtrManager *manager) {
    if (!manager || !manager->aeronaves) return 0;
    return manager->aeronaves->len;
}

const Aeronave* aeronave_ptr_manager_get(const AeronavePtrManager *manager, guint index) {
    if (!manager || !manager->aeronaves || index >= manager->aeronaves->len) return NULL;
    return g_array_index(manager->aeronaves, Aeronave*, index);
}
