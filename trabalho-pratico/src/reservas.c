#include "reservas.h"
#include <glib.h>
#include <stdbool.h>
#include <stdlib.h>

struct Reserva {
	char *reservation_id;
	char *flight_ids;
	char *document_number;
	char *seat;
	double price;
	bool extra_luggage;
	bool priority_boarding;
	char *qr_code;
};

Reserva* reserva_create(const char *reservation_id,
						const char *flight_ids,
						const char *document_number,
						const char *seat,
						double price,
						bool extra_luggage,
						bool priority_boarding,
						const char *qr_code) {
	if (!reservation_id || !flight_ids || !document_number || !seat) return NULL;

	Reserva *reserva = g_new0(Reserva, 1);
	if (!reserva) return NULL;

	reserva->reservation_id = g_strdup(reservation_id);
	reserva->flight_ids = g_strdup(flight_ids);
	reserva->document_number = g_strdup(document_number);
	reserva->seat = g_strdup(seat);
	reserva->price = price;
	reserva->extra_luggage = extra_luggage;
	reserva->priority_boarding = priority_boarding;
	reserva->qr_code = g_strdup(qr_code ? qr_code : "");

	if (!reserva->reservation_id || !reserva->flight_ids ||
		!reserva->document_number || !reserva->seat || !reserva->qr_code) {
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
	g_free(reserva->seat);
	g_free(reserva->qr_code);
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