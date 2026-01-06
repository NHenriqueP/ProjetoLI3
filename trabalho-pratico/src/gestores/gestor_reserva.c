#include "gestor_reserva.h"

#include <glib.h>
#include <stdlib.h>

struct reserva_ptr_manager {
    GArray *reservas; /* array de (Reserva*) */
};

ReservaPtrManager* reserva_ptr_manager_create() {
    ReservaPtrManager *m = malloc(sizeof(ReservaPtrManager));
    if (!m) return NULL;

    m->reservas = g_array_new(FALSE, FALSE, sizeof(Reserva*));
    if (!m->reservas) {
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

    free(manager);
}

int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    ReservaPtrManager *manager = (ReservaPtrManager*)gestor;
    Reserva *reserva = (Reserva*)entidade;
    // Sem hash table, não verificamos duplicados (assume-se que o parser já trata)
    g_array_append_val(manager->reservas, reserva);
    return 1;
}

guint reserva_ptr_manager_count(const ReservaPtrManager *manager) {
    if (!manager || !manager->reservas) return 0;
    return manager->reservas->len;
}

const Reserva* reserva_ptr_manager_get(const ReservaPtrManager *manager, guint index) {
    if (!manager || !manager->reservas || index >= manager->reservas->len) return NULL;
    return g_array_index(manager->reservas, Reserva*, index);
}
