#include "voos.h"

#include <glib.h>
#include <stdlib.h>
#include <ctype.h>

	struct Date {
		int year, month, day;
	};

	struct Hora {
		int hora, minuto;
	};

	struct Horario {
		Date data;
		Hora hora;
	};

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

void horario_copy(Horario *dest, const Horario *src) {
    dest->data = src->data;
    dest->hora = src->hora;
}

void horario_free(Horario *h) {
	if (h) {
		free(h);
	}
}

Date *date_new(void) {
	Date *d = malloc(sizeof(Date));
	if (!d) return NULL;
	memset(d, 0, sizeof(*d));
	return d;
}

Horario *horario_new(void) {
    Horario *h = malloc(sizeof(Horario));
    if (!h) return NULL;
    memset(h, 0, sizeof(*h));
    return h;
}

void horario_reset(Horario *h) {
    if (!h) return;
    memset(h, 0, sizeof(Horario));
}

const Date *horario_get_date(const Horario *h) {
	if (!h) return NULL;
	return &h->data;
}

void set_date(Date *d, int year, int month, int day) {
	if (!d) return;
	d->year = year;
	d->month = month;
	d->day = day;
}

void set_horario(Horario *h, int year, int month, int day, int hora, int minuto) {
	if (!h) return;
	h->data.year = year;
	h->data.month = month;
	h->data.day = day;
	h->hora.hora = hora;
	h->hora.minuto = minuto;
}

int comparar_dates(const Date *a, const Date *b) {
	if (a->year != b->year) return (a->year > b->year) ? 1 : -1;
	if (a->month != b->month) return (a->month > b->month) ? 1 : -1;
	if (a->day != b->day) return (a->day > b->day) ? 1 : -1;
	return 0;
}

int comparar_horarios(const Horario *a, const Horario *b) {
	if (!a || !b) return 0;
	if (a->data.year != b->data.year) return (a->data.year > b->data.year) ? 1 : -1;
	if (a->data.month != b->data.month) return (a->data.month > b->data.month) ? 1 : -1;
	if (a->data.day != b->data.day) return (a->data.day > b->data.day) ? 1 : -1;
	if (a->hora.hora != b->hora.hora) return (a->hora.hora > b->hora.hora) ? 1 : -1;
	if (a->hora.minuto != b->hora.minuto) return (a->hora.minuto > b->hora.minuto) ? 1 : -1;
	return 0;
}

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
