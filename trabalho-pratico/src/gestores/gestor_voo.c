#include "gestor_voo.h"

#include <glib.h>
#include <stdlib.h>

struct voo_ptr_manager {
    GArray *flights;
    GHashTable *by_id;
    GTree *voos_por_data;
    GHashTable *delay_stats_by_airline;
    GHashTable *flight_count_by_aircraft;
	GHashTable *string_pool;
};

static void airline_delay_stats_free(gpointer data) {
    AirlineDelayStats *s = (AirlineDelayStats*)data;
    if (!s) return;
    g_free(s->airline);
    g_free(s);
}

GHashTable *voo_ptr_manager_get_delay_stats_by_airline(VooPtrManager *manager) {
    if (!manager) return NULL;
    return manager->delay_stats_by_airline;
}

long voo_ptr_manager_get_flight_count_by_aircraft(VooPtrManager *manager, const char *aircraft_id) {
    if (!manager || !manager->flight_count_by_aircraft || !aircraft_id || aircraft_id[0] == '\0') return 0;
    const gint *p = g_hash_table_lookup(manager->flight_count_by_aircraft, aircraft_id);
    return p ? (long)(*p) : 0;
}

GTree * manager_get_voos_por_data(VooPtrManager *manager) {
    if (!manager) return NULL;
    return manager->voos_por_data;
}

VooPtrManager* voo_ptr_manager_create() {
    VooPtrManager *m = malloc(sizeof(VooPtrManager));
    if (!m) return NULL;

    m->flights = g_array_new(FALSE, FALSE, sizeof(Voo*));
    m->by_id = g_hash_table_new(g_str_hash, g_str_equal);
    m->voos_por_data = g_tree_new_full((GCompareDataFunc)comparar_dates_dia, NULL, g_free, (GDestroyNotify)g_ptr_array_unref);
	m->delay_stats_by_airline = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, airline_delay_stats_free);
	m->flight_count_by_aircraft = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    m->string_pool = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    if (!m->flights || !m->by_id || !m->voos_por_data || !m->delay_stats_by_airline || !m->flight_count_by_aircraft || !m->string_pool) {
        if (m->flights) g_array_free(m->flights, TRUE);
        if (m->by_id) g_hash_table_destroy(m->by_id);
        if (m->voos_por_data) g_tree_destroy(m->voos_por_data);
		if (m->delay_stats_by_airline) g_hash_table_destroy(m->delay_stats_by_airline);
		if (m->flight_count_by_aircraft) g_hash_table_destroy(m->flight_count_by_aircraft);
		if (m->string_pool) g_hash_table_destroy(m->string_pool);
        free(m);
        return NULL;
    }

    return m;
}

void voo_ptr_manager_destroy(VooPtrManager *manager) {
    if (!manager) return;

    if (manager->flight_count_by_aircraft) {
        g_hash_table_destroy(manager->flight_count_by_aircraft);
        manager->flight_count_by_aircraft = NULL;
    }

    if (manager->flights) {
        for (guint i = 0; i < manager->flights->len; i++) {
            Voo *v = g_array_index(manager->flights, Voo*, i);
            if (v) voo_destroy(v);
        }
        g_array_free(manager->flights, TRUE);
    }

    if (manager->by_id) g_hash_table_destroy(manager->by_id);
    if (manager->voos_por_data) g_tree_destroy(manager->voos_por_data);
	if (manager->delay_stats_by_airline) g_hash_table_destroy(manager->delay_stats_by_airline);
    if (manager->string_pool) {
        g_hash_table_destroy(manager->string_pool);
        manager->string_pool = NULL;
    }
    free(manager);
}

int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    VooPtrManager *manager = (VooPtrManager*)gestor;
    Voo *voo = (Voo*)entidade;
    // Internar strings repetidas (reduz duplicação de memória)
    if (manager->string_pool) {
        voo_intern_repeated_strings(voo, manager->string_pool);
    }
    const char *id = voo_get_id(voo);

    if (manager->by_id && g_hash_table_lookup(manager->by_id, id)) {
        voo_destroy(voo);
        return 1;
    }

    // Atualizar estatísticas de atraso por companhia (apenas DELAYED)
    if (manager->delay_stats_by_airline && voo_get_status(voo) == DELAYED) {
        const char *airline = voo_get_airline(voo);
        if (airline && airline[0] != '\0') {
            AirlineDelayStats *s = g_hash_table_lookup(manager->delay_stats_by_airline, airline);
            if (!s) {
                s = g_new0(AirlineDelayStats, 1);
                s->airline = g_strdup(airline);
                s->delayed_flights_count = 0;
                s->total_delay_minutes = 0;
                if (s->airline) {
                    g_hash_table_insert(manager->delay_stats_by_airline, s->airline, s);
                } else {
                    airline_delay_stats_free(s);
                    s = NULL;
                }
            }
            if (s) {
                s->delayed_flights_count++;
                s->total_delay_minutes += voo_get_departure_delay_minutes(voo);
            }
        }
    }

    // Atualizar contagem de voos por aeronave (exclui CANCELLED), para acelerar a Query 2
    if (manager->flight_count_by_aircraft && voo_get_status(voo) != CANCELLED) {
        const char *aircraft_id = voo_get_aircraft(voo);
        if (aircraft_id && aircraft_id[0] != '\0') {
            gint *p = g_hash_table_lookup(manager->flight_count_by_aircraft, aircraft_id);
            if (!p) {
                char *key_copy = g_strdup(aircraft_id);
                p = g_new0(gint, 1);
                if (key_copy && p) {
                    g_hash_table_insert(manager->flight_count_by_aircraft, key_copy, p);
                } else {
                    if (key_copy) g_free(key_copy);
                    if (p) g_free(p);
                    p = NULL;
                }
            }
            if (p) (*p)++;
        }
    }

    if (voo_get_status(voo) != CANCELLED) {

    Date *data_acDeparture = (Date*)horario_get_date(voo_get_actual_departure(voo));
    Date *key_dia = date_new();
    passar_date(key_dia, data_acDeparture);

    GPtrArray *lista_do_dia = g_tree_lookup(manager->voos_por_data, key_dia);
    
    if (lista_do_dia) {
        g_ptr_array_add(lista_do_dia, voo);
        g_free(key_dia);
    }
    else {
        lista_do_dia = g_ptr_array_new();
        g_ptr_array_add(lista_do_dia, voo);
        g_tree_insert(manager->voos_por_data, key_dia, lista_do_dia);
    }
    
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