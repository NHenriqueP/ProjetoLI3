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
    Status estado;
    char *iataorigem;
    char *iatadestino;
    char *aircraft;
	char *airline;
};

static long long days_from_civil(int y, unsigned m, unsigned d) {
	// Howard Hinnant's algorithm: days since 1970-01-01.
	y -= (m <= 2);
	const int era = (y >= 0 ? y : y - 399) / 400;
	const unsigned yoe = (unsigned)(y - era * 400);
	const unsigned doy = (153 * (m + (m > 2 ? (unsigned)-3 : 9)) + 2) / 5 + d - 1;
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	return (long long)(era * 146097 + (int)doe - 719468);
}

static long long horario_to_minutes(const Horario *h) {
	if (!h) return 0;
	long long days = days_from_civil(h->data.year, (unsigned)h->data.month, (unsigned)h->data.day);
	return days * 1440LL + (long long)h->hora.hora * 60LL + (long long)h->hora.minuto;
}

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

void passar_date (Date *dest, const Date *src) {
	if (!dest || !src) return;
	dest->year = src->year;
	dest->month = src->month;
	dest->day = src->day;
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

// Nova função de comparação para a GTree
gint comparar_dates_dia(gconstpointer a, gconstpointer b) {
    const Date *d1 = (const Date*)a; 
    const Date *d2 = (const Date*)b;

    if (d1->year != d2->year) return d1->year - d2->year;
    if (d1->month != d2->month) return d1->month - d2->month;
    return d1->day - d2->day;
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
	g_free(voo);
}

Voo* voo_create(const char *idvoo,
				const Horario *departure,
				const Horario *actualdeparture,
				Status estado,
				const char *iataorigem,
				const char *iatadestino,
				const char *aircraft,
				const char *airline) {
	if (!idvoo || !departure || !actualdeparture || !iataorigem || !iatadestino || !aircraft || !airline) {
		return NULL;
	}
	Voo *voo = g_new0(Voo, 1);
	if (!voo) return NULL;
	voo->idvoo = g_strdup(idvoo);
	voo->departure = *departure;
	voo->actualdeparture = *actualdeparture;
	voo->estado = estado;
	voo->iataorigem = g_strdup(iataorigem);
	voo->iatadestino = g_strdup(iatadestino);
	voo->aircraft = g_strdup(aircraft);
	voo->airline = g_strdup(airline);
	if (!voo->idvoo || !voo->iataorigem || !voo->iatadestino || !voo->aircraft || !voo->airline) {
		voo_destroy(voo);
		return NULL;
	}
	return voo;
}

const char* voo_get_id(const Voo *voo) {
	if (voo) return voo->idvoo;
	return NULL;
}

const Horario* voo_get_departure(const Voo *voo) {
	return voo ? &voo->departure : NULL;
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

const char* voo_get_airline(const Voo *voo) {
	return voo ? voo->airline : NULL;
}

long long voo_get_departure_delay_minutes(const Voo *voo) {
	if (!voo) return 0;
	// actualdeparture - departure
	return horario_to_minutes(&voo->actualdeparture) - horario_to_minutes(&voo->departure);
}

long long voo_get_departure_day_number(const Voo *voo) {
	if (!voo) return 0;
	// days since 1970-01-01
	return days_from_civil(voo->departure.data.year, (unsigned)voo->departure.data.month, (unsigned)voo->departure.data.day);
}

int validar_flight_id(const char *s) {
	if (!s) return 0;
	size_t len = strlen(s);
	// Fase 2: existem IDs com 7 ou 8 caracteres (2 letras + 5/6 dígitos)
	if (len != 7 && len != 8) return 0;

	for (int j = 0; j < 2; j++) {
		if (!isupper((unsigned char)s[j])) return 0;
	}
	for (size_t i = 2; i < len; i++) {
		if (!isdigit((unsigned char)s[i])) return 0;
	}
	return 1;

}