#ifndef GESTOR_VOO_H
#define GESTOR_VOO_H

#include <glib.h>
#include "voos.h"

typedef struct voo_ptr_manager VooPtrManager;

typedef struct {
	char *airline;
	long delayed_flights_count;
	long long total_delay_minutes;
} AirlineDelayStats;

VooPtrManager* voo_ptr_manager_create();
void voo_ptr_manager_destroy(VooPtrManager *manager);
int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);
Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id);
GTree * manager_get_voos_por_data(VooPtrManager *manager);
guint voo_ptr_manager_count(const VooPtrManager *manager);
const Voo* voo_ptr_manager_get(const VooPtrManager *manager, guint index);

// Tabela com estatísticas agregadas de atrasos por companhia (apenas voos DELAYED).
// Chave: (const char*) airline
// Valor: (AirlineDelayStats*)
// Nota: retorno é propriedade do manager; tratar como read-only.
GHashTable *voo_ptr_manager_get_delay_stats_by_airline(VooPtrManager *manager);

// Contagem de voos por aeronave (exclui CANCELLED), agregada no carregamento.
long voo_ptr_manager_get_flight_count_by_aircraft(VooPtrManager *manager, const char *aircraft_id);

#endif