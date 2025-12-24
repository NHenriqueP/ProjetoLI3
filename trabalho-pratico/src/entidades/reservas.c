#include "reservas.h"
#include <glib.h>
#include <stdbool.h>
#include <stdlib.h>

struct Reserva {
	char *reservation_id; /**/
	char *flight_ids; /**/
	char *document_number; /**/
	double price; /**/
};

Reserva* reserva_create(const char *reservation_id,
						const char *flight_ids,
						const char *document_number,
						double price) {
	if (!reservation_id || !flight_ids || !document_number) return NULL;

	Reserva *reserva = g_new0(Reserva, 1);
	if (!reserva) return NULL;

	reserva->reservation_id = g_strdup(reservation_id);
	reserva->flight_ids = g_strdup(flight_ids);
	reserva->document_number = g_strdup(document_number);
	reserva->price = price;

	if (!reserva->reservation_id || !reserva->flight_ids ||	!reserva->document_number) {
		reserva_destroy(reserva);
		return NULL;
	}

	return reserva;
}

void reserva_destroy(Reserva *reserva) {
	if (!reserva) return;

	g_free(reserva->reservation_id);
	g_free(reserva->flight_ids);
	g_free(reserva->document_number);
	g_free(reserva);
}

const char* reserva_get_reservation_id(const Reserva *reserva) {
	if (reserva) return reserva->reservation_id;
	return NULL;
}

const char* reserva_get_flight_ids(const Reserva *reserva) {
	if (reserva) return reserva->flight_ids;
	return NULL;
}

const char* reserva_get_document_number(const Reserva *reserva) {
	if (reserva) return reserva->document_number;
	return NULL;
}

double reserva_get_price(const Reserva *reserva) {
	return reserva ? reserva->price : 0.0;
}