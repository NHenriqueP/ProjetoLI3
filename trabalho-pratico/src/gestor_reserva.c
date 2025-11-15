#include "gestor_reserva.h"

#include <glib.h>
#include <stdlib.h>

struct reserva_ptr_manager {
    GArray *reservas; /* array de (Reserva*) */
    GHashTable *by_id;
};

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
                reserva_destroy(r);
            }
        }
        g_array_free(manager->reservas, TRUE);
    }

    if (manager->by_id) g_hash_table_destroy(manager->by_id);

    free(manager);
}

int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    ReservaPtrManager *manager = (ReservaPtrManager*)gestor;
    Reserva *reserva = (Reserva*)entidade;
    const char *id = reserva_get_reservation_id(reserva);
    if (manager->by_id && id && g_hash_table_lookup(manager->by_id, id)) {
        reserva_destroy(reserva);
        return 1;
    }

    g_array_append_val(manager->reservas, reserva);
    g_hash_table_insert(manager->by_id, (gpointer)id, reserva);
    return 1;
}
