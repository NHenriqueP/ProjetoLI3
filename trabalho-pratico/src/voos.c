#include "voos.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char* voo_get_id(const Voo *voo) {
    return voo ? voo->idvoo : NULL;
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

const Horario* voo_get_departure(const Voo *voo) {
	return voo ? &voo->departure : NULL;
}

const Horario* voo_get_actual_departure(const Voo *voo) {
	return voo ? &voo->actualdeparture : NULL;
}

const Horario* voo_get_arrival(const Voo *voo) {
	return voo ? &voo->arrival : NULL;
}

const Horario* voo_get_actual_arrival(const Voo *voo) {
	return voo ? &voo->actualarrival : NULL;
}

int voo_get_gate(const Voo *voo) {
	return voo ? voo->gate : -1;
}

const char* voo_get_airline(const Voo *voo) {
	return voo ? voo->airline : NULL;
}

const char* voo_get_tracking_url(const Voo *voo) {
	return voo ? voo->tracking_url : NULL;
}

int voo_summary(const Voo *v, char *buf, size_t bufsize) {
	if (!v || !buf || bufsize == 0) return 0;
	int n = snprintf(buf, bufsize, "%s,%04d-%02d-%02d %02d:%02d,%04d-%02d-%02d %02d:%02d,%s,%s,%s,%s",
					 v->idvoo,
					 v->departure.data.year, v->departure.data.month, v->departure.data.day,
					 v->departure.hora.hora, v->departure.hora.minuto,
					 v->arrival.data.year, v->arrival.data.month, v->arrival.data.day,
					 v->arrival.hora.hora, v->arrival.hora.minuto,
					 v->iataorigem, v->iatadestino, v->aircraft, v->airline);
	if (n < 0) return 0;
	if ((size_t)n >= bufsize) return 0;
	return n;
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
// Parser movido para parser_voo.c
