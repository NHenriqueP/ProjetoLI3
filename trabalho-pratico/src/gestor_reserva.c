#include "gestor_reserva.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct reserva_ptr_manager {
    GArray *reservas; /* array de (Reserva*) */
    GHashTable *by_id;
};


static void free_reserva(Reserva *reserva) {
    if (!reserva) return;

    g_free(reserva->reservation_id);
    g_free(reserva->flight_ids);
    g_free(reserva->document_number);
    g_free(reserva->seat);
    g_free(reserva->qr_code);
}

ReservaPtrManager* reserva_ptr_manager_create() {
    ReservaPtrManager *m = malloc(sizeof(ReservaPtrManager));
    if (!m) return NULL;

    m->reservas = g_array_new(FALSE, FALSE, sizeof(Reserva*));
    m->by_id = g_hash_table_new(g_str_hash, g_str_equal);
    if (!m->reservas || !m->by_id) {
        if (m->reservas) g_array_free(m->reservas, TRUE);
        if (m->by_id) g_hash_table_destroy(m->by_id);
        free(m);
        return NULL;
    }

    return m;
}

void reserva_ptr_manager_destroy(ReservaPtrManager *manager) {
    if (!manager) return;

    if (manager->reservas) {
        for (guint i = 0; i < manager->reservas->len; i++) {
            Reserva *r = g_array_index(manager->reservas, Reserva*, i);
            if (r) {
                free_reserva(r);
                g_free(r);
            }
        }
        g_array_free(manager->reservas, TRUE);
    }

    if (manager->by_id) g_hash_table_destroy(manager->by_id);

    free(manager);
}

int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade) {
    if (!manager || !entidade) return 0;
    if (manager->by_id && g_hash_table_lookup(manager->by_id, entidade->reservation_id)) {
        free_reserva(entidade);
        g_free(entidade);
        return 1;
    }

    g_array_append_val(manager->reservas, entidade);
    g_hash_table_insert(manager->by_id, entidade->reservation_id, entidade);
    return 1;
}

int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return reserva_ptr_manager_add_from_ptr((ReservaPtrManager*)gestor, (Reserva*)entidade);
}

Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id) {
    if (!manager || !manager->by_id || !reservation_id) return NULL;
    return g_hash_table_lookup(manager->by_id, reservation_id);
}

Reserva* reserva_ptr_manager_find_by_document(ReservaPtrManager *manager, const char *document_number) {
    if (!manager || !manager->reservas || !document_number) return NULL;

    for (guint i = 0; i < manager->reservas->len; i++) {
        Reserva *r = g_array_index(manager->reservas, Reserva*, i);
        if (r && r->document_number && strcmp(r->document_number, document_number) == 0) {
            return r;
        }
    }

    return NULL;
}

guint reserva_ptr_manager_count(const ReservaPtrManager *manager) {
    if (!manager || !manager->reservas) return 0;
    return manager->reservas->len;
}

const Reserva* reserva_ptr_manager_get(const ReservaPtrManager *manager, guint index) {
    if (!manager || !manager->reservas || index >= manager->reservas->len) return NULL;
    return g_array_index(manager->reservas, Reserva*, index);
}
