#include "gestor_passageiro.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct PassageiroManager {
    GHashTable *passageiros_by_doc;
    GArray *passageiros_list;
    int count;
};

struct passageiro_ptr_manager {
    GArray *passageiros; /* array de (Passageiro*) */
};

static PassageiroManager *mgr = NULL;

static void free_passageiro(Passageiro *passageiro) {
    if (!passageiro) return;

    g_free(passageiro->document_number);
    g_free(passageiro->first_name);
    g_free(passageiro->last_name);
    g_free(passageiro->dob);
    g_free(passageiro->nationality);
    g_free(passageiro->gender);
    g_free(passageiro->email);
    g_free(passageiro->phone);
    g_free(passageiro->address);
    g_free(passageiro->photo);
}

PassageiroManager* passageiro_manager_create() {
    PassageiroManager *manager = malloc(sizeof(PassageiroManager));
    if (!manager) return NULL;

    manager->count = 0;
    manager->passageiros_by_doc = g_hash_table_new(g_str_hash, g_str_equal);
    manager->passageiros_list = g_array_new(FALSE, FALSE, sizeof(Passageiro*));

    if (!manager->passageiros_by_doc || !manager->passageiros_list) {
        if (manager->passageiros_by_doc) g_hash_table_destroy(manager->passageiros_by_doc);
        if (manager->passageiros_list) g_array_free(manager->passageiros_list, TRUE);
        free(manager);
        return NULL;
    }

    return manager;
}

void init_passageiro_manager() {
    if (!mgr) {
        mgr = passageiro_manager_create();
    }
}

PassageiroManager* get_passageiro_manager() {
    return mgr;
}

int passageiro_manager_add(PassageiroManager *manager, Passageiro passageiro) {
    if (!manager) return 0;

    Passageiro *new_passageiro = g_new(Passageiro, 1);
    if (!new_passageiro) return 0;

    *new_passageiro = passageiro;

    g_hash_table_insert(manager->passageiros_by_doc, new_passageiro->document_number, new_passageiro);
    g_array_append_val(manager->passageiros_list, new_passageiro);

    manager->count++;
    return 1;
}

Passageiro* passageiro_manager_find_by_doc(PassageiroManager *manager, const char *document_number) {
    if (!manager || !document_number) return NULL;
    return (Passageiro*)g_hash_table_lookup(manager->passageiros_by_doc, document_number);
}

Passageiro* passageiro_manager_find_by_email(PassageiroManager *manager, const char *email) {
    if (!manager || !manager->passageiros_list || !email) return NULL;

    for (guint i = 0; i < manager->passageiros_list->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros_list, Passageiro*, i);
        if (p && p->email && strcmp(p->email, email) == 0) {
            return p;
        }
    }
    return NULL;
}

int passageiro_exists_by_doc(PassageiroManager *manager, const char *document_number) {
    if (!manager || !document_number) return 0;
    return g_hash_table_contains(manager->passageiros_by_doc, document_number);
}

int passageiro_manager_get_count(PassageiroManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

Passageiro* passageiro_manager_get_at_index(PassageiroManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->passageiros_list, Passageiro*, index);
}

void passageiro_manager_destroy(PassageiroManager *manager) {
    if (!manager) return;

    for (guint i = 0; i < manager->passageiros_list->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros_list, Passageiro*, i);
        if (p) {
            free_passageiro(p);
            g_free(p);
        }
    }

    g_hash_table_destroy(manager->passageiros_by_doc);
    g_array_free(manager->passageiros_list, TRUE);

    free(manager);
}

void cleanup_passageiro_manager() {
    if (mgr) {
        passageiro_manager_destroy(mgr);
        mgr = NULL;
    }
}

PassageiroPtrManager* passageiro_ptr_manager_create() {
    PassageiroPtrManager *m = malloc(sizeof(PassageiroPtrManager));
    if (!m) return NULL;

    m->passageiros = g_array_new(FALSE, FALSE, sizeof(Passageiro*));
    if (!m->passageiros) {
        free(m);
        return NULL;
    }

    return m;
}

void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager) {
    if (!manager) return;

    if (manager->passageiros) {
        for (guint i = 0; i < manager->passageiros->len; i++) {
            Passageiro *p = g_array_index(manager->passageiros, Passageiro*, i);
            if (p) {
                free_passageiro(p);
                g_free(p);
            }
        }
        g_array_free(manager->passageiros, TRUE);
    }

    free(manager);
}

int passageiro_ptr_manager_add_from_ptr(PassageiroPtrManager *manager, Passageiro *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->passageiros, entidade);
    return 1;
}

int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return passageiro_ptr_manager_add_from_ptr((PassageiroPtrManager*)gestor, (Passageiro*)entidade);
}

Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number) {
    if (!manager || !manager->passageiros || !document_number) return NULL;

    for (guint i = 0; i < manager->passageiros->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros, Passageiro*, i);
        if (p && p->document_number && strcmp(p->document_number, document_number) == 0) return p;
    }

    return NULL;
}
