#include "voos.h"
#include "csv_parser.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gestorglobal.h"
#include <ctype.h>

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

static int parse_horario(const char *s, Horario *h) {
    if (!s || !h) return 0;
    int Y,M,D,hh,mm;
    if (sscanf(s, "%d-%d-%d %d:%d", &Y, &M, &D, &hh, &mm) != 5) return 0;
    if (Y < 1900 || M < 1 || M > 12 || D < 1 || D > 31 || hh < 0 || hh > 23 || mm < 0 || mm > 59) return 0;
    h->data.year = Y; h->data.month = M; h->data.day = D;
    h->hora.hora = hh; h->hora.minuto = mm;
    return 1;
}

static int comparar_horario(const Horario *a, const Horario *b) {
	if (!a || !b) return 0;
	if (a->data.year != b->data.year) return (a->data.year > b->data.year) ? 1 : -1;
	if (a->data.month != b->data.month) return (a->data.month > b->data.month) ? 1 : -1;
	if (a->data.day != b->data.day) return (a->data.day > b->data.day) ? 1 : -1;
	if (a->hora.hora != b->hora.hora) return (a->hora.hora > b->hora.hora) ? 1 : -1;
	if (a->hora.minuto != b->hora.minuto) return (a->hora.minuto > b->hora.minuto) ? 1 : -1;
	return 0;
}

static int parse_horario_optional(const char *s, Horario *h, int *tem_valor) {
	if (!h || !tem_valor || !s) return 0;
	if (s[0] == '\0') return 0;
	if (g_ascii_strcasecmp(s, "N/A") == 0 || g_ascii_strcasecmp(s, "NA") == 0) {
		*h = (Horario){0};
		*tem_valor = 0;
		return 1;
	}
	*tem_valor = 1;
	return parse_horario(s, h);
}

static Status parse_status(const char *s, int *ok) {
    *ok = 1;
    if (!s) { *ok = 0; return ON_TIME; }
    if (g_ascii_strcasecmp(s, "ON_TIME") == 0 || g_ascii_strcasecmp(s, "ON TIME") == 0 || g_ascii_strcasecmp(s, "On time") == 0) return ON_TIME;
    if (g_ascii_strcasecmp(s, "DELAYED") == 0 || g_ascii_strcasecmp(s, "Delayed") == 0) return DELAYED;
    if (g_ascii_strcasecmp(s, "CANCELLED") == 0 || g_ascii_strcasecmp(s, "Cancelled") == 0) return CANCELLED;
    *ok = 0; return ON_TIME;
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

static int validar_url_http(const char *s) {
	if (!s) return 0;
	return g_str_has_prefix(s, "http://") || g_str_has_prefix(s, "https://");
}

ParseResultado parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor) {
	ParseResultado r; r.valido = false; r.entidade = NULL;
	if (n_fields < 12) { return r; }

	const char *flight_id   = fields[0];
	const char *dep_str     = fields[1];
	const char *act_dep_str = fields[2];
	const char *arr_str     = fields[3];
	const char *act_arr_str = fields[4];
	const char *gate_str    = fields[5];
	const char *status_str  = fields[6];
	const char *origem      = fields[7];
	const char *destino     = fields[8];
	const char *aircraft    = fields[9];
	const char *airline     = fields[10];
	const char *url         = fields[11];

	if (!validar_flight_id(flight_id)) { return r; }
	if (!validar_iata(origem) || !validar_iata(destino)) { return r; }
	if (strcmp(origem, destino) == 0) { return r; }
	if (!aircraft || strlen(aircraft) >= sizeof(((Voo*)0)->aircraft)) { return r; }
	if (!airline || strlen(airline) >= sizeof(((Voo*)0)->airline)) { return r; }

	char *endp = NULL; long gate = strtol(gate_str ? gate_str : "", &endp, 10);
	if (!gate_str || endp == gate_str || gate <= 0) { return r; }

	Horario dep = {0}, act_dep = (Horario){0}, arr = {0}, act_arr = (Horario){0};
	if (!parse_horario(dep_str, &dep)) { return r; }
	if (!parse_horario(arr_str, &arr)) { return r; }
	// O arrival agendado tem de ocorrer depois do departure agendado
	if (comparar_horario(&arr, &dep) <= 0) { return r; }
	int tem_act_dep = 0;
	int tem_act_arr = 0;
	if (!parse_horario_optional(act_dep_str, &act_dep, &tem_act_dep)) { return r; }
	if (!parse_horario_optional(act_arr_str, &act_arr, &tem_act_arr)) { return r; }
	if (!tem_act_dep) act_dep = dep;
	if (!tem_act_arr) act_arr = arr;
	if (tem_act_dep && tem_act_arr && comparar_horario(&act_arr, &act_dep) <= 0) {
		return r;
	}

	int ok_status = 0; Status st = parse_status(status_str, &ok_status);
	if (!ok_status) { return r; }

	Voo *v = g_new0(Voo, 1);
	if (!v) { return r; }

	// Copiar para buffers fixos garantindo terminação
	g_strlcpy(v->idvoo, flight_id, sizeof(v->idvoo));
	v->departure = dep;
	v->actualdeparture = act_dep;
	v->arrival = arr;
	v->actualarrival = act_arr;
	v->gate = (int)gate;
	v->estado = st;
	g_strlcpy(v->iataorigem, origem, sizeof(v->iataorigem));
	g_strlcpy(v->iatadestino, destino, sizeof(v->iatadestino));
	g_strlcpy(v->aircraft, aircraft, sizeof(v->aircraft));
	g_strlcpy(v->airline, airline, sizeof(v->airline));
	if (!validar_url_http(url)) { g_free(v); return r; }
	v->tracking_url = g_strdup(url);

	r.valido = true;
	r.entidade = v;
	return r;
}
