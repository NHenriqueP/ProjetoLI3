#ifndef RESERVAS
#define RESERVAS

#include <stddef.h>
#include <stdbool.h>

typedef struct reserva {
	char *reservation_id;
	char *flight_ids;
	char *document_number;
	char *seat;
	double price;
	bool extra_luggage;
	bool priority_boarding;
	char *qr_code;
} Reserva;

int reserva_summary(const Reserva *reserva, char *buf, size_t bufsize);

const char* reserva_get_id(const Reserva *r);
const char* reserva_get_flight_ids(const Reserva *r);
const char* reserva_get_document_number(const Reserva *r);
const char* reserva_get_seat(const Reserva *r);
double reserva_get_price(const Reserva *r);
bool reserva_get_extra_luggage(const Reserva *r);
bool reserva_get_priority_boarding(const Reserva *r);
const char* reserva_get_qr_code(const Reserva *r);

#endif