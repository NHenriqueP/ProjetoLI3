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

#endif