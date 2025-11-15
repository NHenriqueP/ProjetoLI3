#include "voos.h"

#include <glib.h>
#include <stdlib.h>
#include <ctype.h>
struct Voo {
    char *idvoo;
    Horario departure;
    Horario actualdeparture;
    Horario arrival;
    Horario actualarrival;
    int gate;
    Status estado;
    char *iataorigem;
    char *iatadestino;
    char *aircraft;
    char *airline;
    char *tracking_url;
};
void voo_destroy(Voo *voo) {
	if (!voo) return;

	g_free(voo->idvoo);
	g_free(voo->iataorigem);
	g_free(voo->iatadestino);
	g_free(voo->aircraft);
	g_free(voo->airline);
	g_free(voo->tracking_url);
	g_free(voo);
}

Voo* voo_create(const char *idvoo,
				const Horario *departure,
				const Horario *actualdeparture,
				const Horario *arrival,
				const Horario *actualarrival,
				int gate,
				Status estado,
				const char *iataorigem,
				const char *iatadestino,
				const char *aircraft,
				const char *airline,
				const char *tracking_url) {
	if (!idvoo || !departure || !arrival || !iataorigem || !iatadestino || !aircraft || !airline || !tracking_url) {
		return NULL;
	}
	Voo *voo = g_new0(Voo, 1);
	if (!voo) return NULL;
	voo->idvoo = g_strdup(idvoo);
	voo->departure = *departure;
	voo->actualdeparture = *actualdeparture;
	voo->arrival = *arrival;
	voo->actualarrival = *actualarrival;
	voo->gate = gate;
	voo->estado = estado;
	voo->iataorigem = g_strdup(iataorigem);
	voo->iatadestino = g_strdup(iatadestino);
	voo->aircraft = g_strdup(aircraft);
	voo->airline = g_strdup(airline);
	voo->tracking_url = g_strdup(tracking_url);
	if (!voo->idvoo || !voo->iataorigem || !voo->iatadestino || !voo->aircraft || !voo->airline || !voo->tracking_url) {
		voo_destroy(voo);
		return NULL;
	}
	return voo;
}

const char* voo_get_id(const Voo *voo) {
	if (voo) return voo->idvoo;
	return NULL;
}


Status voo_get_status(const Voo *voo) {
	return voo ? voo->estado : ON_TIME;
}

const char* voo_get_aircraft(const Voo *voo) {
    return voo ? voo->aircraft : NULL;
}

const char* voo_get_iata_origem(const Voo *voo) {
    return voo ? voo->iataorigem : NULL;
}

const char* voo_get_iata_destino(const Voo *voo) {
    return voo ? voo->iatadestino : NULL;
}

const Horario* voo_get_actual_departure(const Voo *voo) {
	return voo ? &voo->actualdeparture : NULL;
}

int validar_flight_id(const char *s) {
	if (!s || strlen(s) != 7) return 0;
	
	for (int j = 0; j < 2; j++) {
		if (!isupper((unsigned char)s[j])) return 0;
	}
	for (int i = 2; i < 7; i++) {
		if (!isdigit((unsigned char)s[i])) return 0;
	}
	return 1;

}
