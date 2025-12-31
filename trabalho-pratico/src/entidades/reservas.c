#include "reservas.h"
#include <glib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static int extract_flight_ids_compact(const char *src, char out_ids[][9], int max_ids) {
	if (!src || !out_ids || max_ids <= 0) return 0;
	int count = 0;
	const char *p = src;
	while (*p && count < max_ids) {
		// procurar início de token com 2 letras maiúsculas
		while (*p && !(g_ascii_isupper((guchar)p[0]) && g_ascii_isupper((guchar)p[1]))) p++;
		if (!*p || !p[1]) break;
		// copiar 2 letras + 5/6 dígitos (total 7 ou 8)
		int i = 0;
		out_ids[count][i++] = *p++;
		out_ids[count][i++] = *p++;
		int digit_count = 0;
		while (*p && g_ascii_isdigit((guchar)*p) && digit_count < 6 && i < 8) {
			out_ids[count][i++] = *p++;
			digit_count++;
		}
		// aceitar exatamente 5 ou 6 dígitos
		if (digit_count != 5 && digit_count != 6) {
			// token inválido: continuar a procura mais à frente
			continue;
		}
		out_ids[count][i] = '\0';
		count++;
	}
	return count;
}

struct Reserva {
	char *reservation_id; /**/
	char *document_number; /**/
	double price; /**/
	guint8 n_flights;
	char flight_ids_compact[2][9];
	char *flight_ids_raw_cache;
};

Reserva* reserva_create(const char *reservation_id,
						const char *flight_ids,
						const char *document_number,
						double price) {
	if (!reservation_id || !flight_ids || !document_number) return NULL;
	char ids[2][9] = {{0}};
	int n = extract_flight_ids_compact(flight_ids, ids, 2);
	if (n < 1 || n > 2) return NULL;
	return reserva_create_from_ids(reservation_id, ids, n, document_number, price);
}

Reserva* reserva_create_from_ids(const char *reservation_id,
							const char flight_ids[][9],
							int n_flights,
							const char *document_number,
							double price) {
	if (!reservation_id || !document_number || !flight_ids) return NULL;
	if (n_flights < 1 || n_flights > 2) return NULL;

	Reserva *reserva = g_new0(Reserva, 1);
	if (!reserva) return NULL;

	reserva->reservation_id = g_strdup(reservation_id);
	reserva->document_number = g_strdup(document_number);
	reserva->price = price;
	reserva->n_flights = (guint8)n_flights;
	memset(reserva->flight_ids_compact, 0, sizeof(reserva->flight_ids_compact));
	for (int i = 0; i < n_flights; i++) {
		g_strlcpy(reserva->flight_ids_compact[i], flight_ids[i], 9);
	}
	reserva->flight_ids_raw_cache = NULL;

	if (!reserva->reservation_id || !reserva->document_number) {
		reserva_destroy(reserva);
		return NULL;
	}

	return reserva;
}

void reserva_destroy(Reserva *reserva) {
	if (!reserva) return;

	g_free(reserva->reservation_id);
	g_free(reserva->document_number);
	g_free(reserva->flight_ids_raw_cache);
	g_free(reserva);
}

const char* reserva_get_reservation_id(const Reserva *reserva) {
	if (reserva) return reserva->reservation_id;
	return NULL;
}

const char* reserva_get_flight_ids(const Reserva *reserva) {
	if (!reserva) return NULL;
	if (reserva->flight_ids_raw_cache) return reserva->flight_ids_raw_cache;
	// Reconstruir no formato do CSV: "['ID1', 'ID2']" (ou só um)
	if (reserva->n_flights == 0) return NULL;
	GString *s = g_string_new(NULL);
	if (!s) return NULL;
	g_string_append(s, "[");
	for (guint i = 0; i < reserva->n_flights; i++) {
		if (i > 0) g_string_append(s, ", ");
		g_string_append_c(s, '\'');
		g_string_append(s, reserva->flight_ids_compact[i]);
		g_string_append_c(s, '\'');
	}
	g_string_append(s, "]");
	((Reserva*)reserva)->flight_ids_raw_cache = g_string_free(s, FALSE);
	return reserva->flight_ids_raw_cache;
}

int reserva_get_n_flights(const Reserva *reserva) {
	if (!reserva) return 0;
	return (int)reserva->n_flights;
}

const char* reserva_get_flight_id_at(const Reserva *reserva, int index) {
	if (!reserva) return NULL;
	if (index < 0 || index >= (int)reserva->n_flights) return NULL;
	return reserva->flight_ids_compact[index][0] ? reserva->flight_ids_compact[index] : NULL;
}

const char* reserva_get_document_number(const Reserva *reserva) {
	if (reserva) return reserva->document_number;
	return NULL;
}

double reserva_get_price(const Reserva *reserva) {
	return reserva ? reserva->price : 0.0;
}