#include "gestor_passageiro.h"

#include <glib.h>
#include <stdlib.h>

struct passageiro_ptr_manager {
    GArray *passageiros; /* array de (Passageiro*) */
    GHashTable *by_doc;
};

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

PassageiroPtrManager* passageiro_ptr_manager_create() {
    PassageiroPtrManager *m = malloc(sizeof(PassageiroPtrManager));
    if (!m) return NULL;

    m->passageiros = g_array_new(FALSE, FALSE, sizeof(Passageiro*));
    m->by_doc = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->passageiros || !m->by_doc) {
        if (m->passageiros) g_array_free(m->passageiros, TRUE);
        if (m->by_doc) g_hash_table_destroy(m->by_doc);
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

    if (manager->by_doc) g_hash_table_destroy(manager->by_doc);

    free(manager);
}

int existe_document_number(PassageiroPtrManager *manager, const char *document_number) {
    if (!manager || !document_number) return 0;
    return g_hash_table_contains(manager->by_doc, document_number);
}

int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    PassageiroPtrManager *manager = (PassageiroPtrManager*)gestor;
    Passageiro *passageiro = (Passageiro*)entidade;
    if (manager->by_doc && g_hash_table_lookup(manager->by_doc, passageiro->document_number)) {
        free_passageiro(passageiro);
        g_free(passageiro);
        return 1;
    }
    g_array_append_val(manager->passageiros, passageiro);
    g_hash_table_insert(manager->by_doc, passageiro->document_number, passageiro);
    return 1;
}
