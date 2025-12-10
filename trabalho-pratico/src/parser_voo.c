#include "parser_voo.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "gestor_aeronave.h"
#include "gestor_voo.h"
#include "parser_aeroporto.h"

static int parse_horario(const char *s, Horario *h) {
    if (!s || !h) return 0;
    int Y, M, D, hh, mm;
    if (sscanf(s, "%d-%d-%d %d:%d", &Y, &M, &D, &hh, &mm) != 5) return 0;
    if (Y < 1900 || M < 1 || M > 12 || D < 1 || D > 31 || hh < 0 || hh > 23 || mm < 0 || mm > 59) return 0;
    set_horario(h, Y, M, D, hh, mm);
    return 1;
}

static int parse_horario_optional(const char *s, Horario *h, int *tem_valor) {
    if (!h || !tem_valor || !s) return 0;
    if (s[0] == '\0') return 0;
    if (g_ascii_strcasecmp(s, "N/A") == 0) {
        horario_reset(h);
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

void* criar_voo(char **fields, int n_fields, void *ctx) {
    SistemaGestor *gestor = (SistemaGestor*)ctx;
    if (!gestor || n_fields < 12) return NULL;

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

    if (!validar_flight_id(flight_id)) return NULL;
    if (!validar_iata(origem) || !validar_iata(destino)) return NULL;
    if (strcmp(origem, destino) == 0) return NULL;
    if (!aircraft || !existe_aeronave(gestor_global_get_aeronaves(gestor), aircraft)) return NULL;
    if (!airline) return NULL;

    char *endp = NULL;
    long gate = strtol(gate_str ? gate_str : "", &endp, 10);
    if (!gate_str || endp == gate_str || gate <= 0) return NULL;

    Horario *dep = horario_new();
    Horario *act_dep = horario_new();
    Horario *arr = horario_new();
    Horario *act_arr = horario_new();
    if (!dep || !act_dep || !arr || !act_arr) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    if (!parse_horario(dep_str, dep) || !parse_horario(arr_str, arr)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }
    if (comparar_horarios(arr, dep) <= 0) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    int tem_act_dep = 0;
    int tem_act_arr = 0;
    if (!parse_horario_optional(act_dep_str, act_dep, &tem_act_dep)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }
    if (!parse_horario_optional(act_arr_str, act_arr, &tem_act_arr)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    int ok_status = 0;
    Status st = parse_status(status_str, &ok_status);
    if (!ok_status) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    if (st == CANCELLED) {
        if (tem_act_dep || tem_act_arr) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return NULL;
        }
    } else if (st == DELAYED) {
        if (tem_act_dep && comparar_horarios(act_dep, dep) < 0) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return NULL;
        }
        if (tem_act_arr && comparar_horarios(act_arr, arr) < 0) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return NULL;
        }
    }

    if (!tem_act_dep) horario_copy(act_dep, dep);
    if (!tem_act_arr) horario_copy(act_arr, arr);
    if (tem_act_dep && tem_act_arr && comparar_horarios(act_arr, act_dep) <= 0) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    if (!validar_url_http(url)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return NULL;
    }

    Voo *v = voo_create(flight_id, dep, act_dep, arr, act_arr, (int)gate, st, origem, destino, aircraft, airline, url);
    horario_free(dep);
    horario_free(act_dep);
    horario_free(arr);
    horario_free(act_arr);
    return v;
}

ParseResultado* parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    Voo *v = (Voo*)criar_voo(fields, n_fields, gestor);
    if (!v) return r;

    parse_resultado_mark_valid(r, v);
    return r;
}

int csv_parser_voos(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    SistemaGestor *gestorGlob,
    char separador
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(caminho_csv ? caminho_csv : "<NULL>", stderr);
        fputs("': ", stderr);
        fputs(strerror(errno), stderr);
        fputc('\n', stderr);
        return 0;
    }

    FILE *ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
    if (!ficheiro_erros_ptr) {
        fputs("Erro ao criar ficheiro de erros: ", stderr);
        fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
        fputc('\n', stderr);
        fclose(ficheiro);
        return 0;
    }

    int contador_validos = csv_generic_parser(
        ficheiro,
        ficheiro_erros_ptr,
        gestor,
        gestorGlob,
        NULL,
        criar_voo,
        voo_ptr_manager_add_from_ptr_adapter,
        separador
    );

    fclose(ficheiro_erros_ptr);
    fclose(ficheiro);

    return contador_validos;
}