#include "gestor_reserva.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct ReservaManager {
    GHashTable *reservas_by_id;
    GArray *reservas_list;
    int count;
};

struct reserva_ptr_manager {
    GArray *reservas; /* array de (Reserva*) */
};

static ReservaManager *mgr = NULL;

static void free_reserva(Reserva *reserva) {
    if (!reserva) return;

    g_free(reserva->reservation_id);
    g_free(reserva->flight_ids);
    g_free(reserva->document_number);
    g_free(reserva->seat);
    g_free(reserva->qr_code);
}

ReservaManager* reserva_manager_create() {
    ReservaManager *manager = malloc(sizeof(ReservaManager));
    if (!manager) return NULL;

    manager->count = 0;
    manager->reservas_by_id = g_hash_table_new(g_str_hash, g_str_equal);
    manager->reservas_list = g_array_new(FALSE, FALSE, sizeof(Reserva*));

    if (!manager->reservas_by_id || !manager->reservas_list) {
        if (manager->reservas_by_id) g_hash_table_destroy(manager->reservas_by_id);
        if (manager->reservas_list) g_array_free(manager->reservas_list, TRUE);
        free(manager);
        return NULL;
    }

    return manager;
}

void init_reserva_manager() {
    if (!mgr) {
        mgr = reserva_manager_create();
    }
}

ReservaManager* get_reserva_manager() {
    return mgr;
}

int reserva_manager_add(ReservaManager *manager, Reserva reserva) {
    if (!manager) return 0;

    Reserva *new_reserva = g_new(Reserva, 1);
    if (!new_reserva) return 0;

    *new_reserva = reserva;

    g_hash_table_insert(manager->reservas_by_id, new_reserva->reservation_id, new_reserva);
    g_array_append_val(manager->reservas_list, new_reserva);

    manager->count++;
    return 1;
}

Reserva* reserva_manager_find_by_id(ReservaManager *manager, const char *reservation_id) {
    if (!manager || !reservation_id) return NULL;
    return (Reserva*)g_hash_table_lookup(manager->reservas_by_id, reservation_id);
}

Reserva* reserva_manager_find_by_document(ReservaManager *manager, const char *document_number) {
    if (!manager || !manager->reservas_list || !document_number) return NULL;

    for (guint i = 0; i < manager->reservas_list->len; i++) {
        Reserva *r = g_array_index(manager->reservas_list, Reserva*, i);
        if (r && r->document_number && strcmp(r->document_number, document_number) == 0) {
            return r;
        }
    }
    return NULL;
}

int reserva_exists_by_id(ReservaManager *manager, const char *reservation_id) {
    if (!manager || !reservation_id) return 0;
    return g_hash_table_contains(manager->reservas_by_id, reservation_id);
}

int reserva_manager_get_count(ReservaManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

Reserva* reserva_manager_get_at_index(ReservaManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->reservas_list, Reserva*, index);
}

void reserva_manager_destroy(ReservaManager *manager) {
    if (!manager) return;

    for (guint i = 0; i < manager->reservas_list->len; i++) {
        Reserva *r = g_array_index(manager->reservas_list, Reserva*, i);
        if (r) {
            free_reserva(r);
            g_free(r);
        }
    }

    g_hash_table_destroy(manager->reservas_by_id);
    g_array_free(manager->reservas_list, TRUE);

    free(manager);
}

void cleanup_reserva_manager() {
    if (mgr) {
        reserva_manager_destroy(mgr);
        mgr = NULL;
    }
}

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
                free_reserva(r);
                g_free(r);
            }
        }
        g_array_free(manager->reservas, TRUE);
    }

    free(manager);
}

int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->reservas, entidade);
    return 1;
}

int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return reserva_ptr_manager_add_from_ptr((ReservaPtrManager*)gestor, (Reserva*)entidade);
}

Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id) {
    if (!manager || !manager->reservas || !reservation_id) return NULL;

    for (guint i = 0; i < manager->reservas->len; i++) {
        Reserva *r = g_array_index(manager->reservas, Reserva*, i);
        if (r && r->reservation_id && strcmp(r->reservation_id, reservation_id) == 0) return r;
    }

    return NULL;
}
