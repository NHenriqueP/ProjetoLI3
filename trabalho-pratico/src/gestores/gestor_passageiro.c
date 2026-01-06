#include "gestor_passageiro.h"
#include "passageiros.h"

#include <glib.h>
#include <stdlib.h>

struct passageiro_ptr_manager {
    GHashTable *by_doc; /* key: doc_number (owned by Passageiro), value: Passageiro* */
};

PassageiroPtrManager* passageiro_ptr_manager_create() {
    PassageiroPtrManager *m = malloc(sizeof(PassageiroPtrManager));
    if (!m) return NULL;

    m->by_doc = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->by_doc) {
        free(m);
        return NULL;
    }

    return m;
}

void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager) {
    if (!manager) return;

    if (manager->by_doc) {
        // Destruir todos os passageiros ao destruir a hash table
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, manager->by_doc);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            Passageiro *p = (Passageiro*)value;
            if (p) passageiro_destroy(p);
        }
        g_hash_table_destroy(manager->by_doc);
    }

    free(manager);
}

int existe_document_number(PassageiroPtrManager *manager, const char *document_number) {
    if (!manager || !document_number) return 0;
    return g_hash_table_contains(manager->by_doc, document_number);
}

Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number) {
	if (!manager || !manager->by_doc || !document_number) return NULL;
	return g_hash_table_lookup(manager->by_doc, document_number);
}

int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    PassageiroPtrManager *manager = (PassageiroPtrManager*)gestor;
    Passageiro *passageiro = (Passageiro*)entidade;
    const char *doc_number = get_passageiro_document_number(passageiro);
    if (manager->by_doc && g_hash_table_lookup(manager->by_doc, doc_number)) {
        passageiro_destroy(passageiro);
        return 1;
    }
    g_hash_table_insert(manager->by_doc, (gpointer)doc_number, passageiro);
    return 1;
}