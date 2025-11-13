#include "parser_voo.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gestor_aeronave.h"

static int parse_horario(const char *s, Horario *h) {
    if (!s || !h) return 0;
    int Y, M, D, hh, mm;
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

static int validar_url_http(const char *s) {
    if (!s) return 0;
    return g_str_has_prefix(s, "http://") || g_str_has_prefix(s, "https://");
}

ParseResultado parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;

    (void)gestor;

    if (n_fields < 12) {
        return r;
    }

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

    if (!validar_flight_id(flight_id)) {
        return r;
    }
    if (!validar_iata(origem) || !validar_iata(destino)) {
        return r;
    }
    if (strcmp(origem, destino) == 0) {
        return r;
    }
    if (!aircraft || strlen(aircraft) >= sizeof(((Voo*)0)->aircraft) || !existe_aeronave(gestor_global_get_aeronaves(gestor), aircraft)) {
        return r;
    }
    if (!airline || strlen(airline) >= sizeof(((Voo*)0)->airline)) {
        return r;
    }

    char *endp = NULL;
    long gate = strtol(gate_str ? gate_str : "", &endp, 10);
    if (!gate_str || endp == gate_str || gate <= 0) {
        return r;
    }

    Horario dep = {0}, act_dep = (Horario){0}, arr = {0}, act_arr = (Horario){0};
    if (!parse_horario(dep_str, &dep)) {
        return r;
    }
    if (!parse_horario(arr_str, &arr)) {
        return r;
    }
    if (comparar_horario(&arr, &dep) <= 0) {
        return r;
    }
    int tem_act_dep = 0;
    int tem_act_arr = 0;
    if (!parse_horario_optional(act_dep_str, &act_dep, &tem_act_dep)) {
        return r;
    }
    if (!parse_horario_optional(act_arr_str, &act_arr, &tem_act_arr)) {
        return r;
    }
    if (!tem_act_dep) act_dep = dep;
    if (!tem_act_arr) act_arr = arr;
    if (tem_act_dep && tem_act_arr && comparar_horario(&act_arr, &act_dep) <= 0) {
        return r;
    }

    int ok_status = 0;
    Status st = parse_status(status_str, &ok_status);
    if (!ok_status) {
        return r;
    }

    Voo *v = g_new0(Voo, 1);
    if (!v) {
        return r;
    }

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
    if (!validar_url_http(url)) {
        g_free(v);
        return r;
    }
    v->tracking_url = g_strdup(url);

    r.valido = true;
    r.entidade = v;
    return r;
}
