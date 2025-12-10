#include "parser_reserva.h"
#include <ctype.h>
#include <errno.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int validar_reservation_id(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    if (s[0] != 'R') return 0;
    for (size_t i = 1; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

static int validar_seat(const char *s) {
    if (!s || strlen(s) < 2) return 0;
    int i = 0;
    if (!isdigit((unsigned char)s[i])) return 0;
    while (s[i] != '\0' && isdigit((unsigned char)s[i])) {
        i++;
    }
    if (s[i] == '\0' || !isalpha((unsigned char)s[i])) return 0;
    i++;
    if (s[i] != '\0') return 0;
    return 1;
}

static int validar_preco(const char *s, double *out) {
    if (!s || !out) return 0;
    char *endptr;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return 0;
    if (val < 0) return 0;
    *out = val;
    return 1;
}

static int parse_boolean(const char *s, bool *out) {
    if (!s || !out) return 0;
    if (g_ascii_strcasecmp(s, "true") == 0 || strcmp(s, "1") == 0) { *out = true; return 1; }
    if (g_ascii_strcasecmp(s, "false") == 0 || strcmp(s, "0") == 0) { *out = false; return 1; }
    return 0;
}

static char* trim_spaces(char *s) {
    if (!s) return s;
    while (g_ascii_isspace(*s)) s++;
    char *end = s + strlen(s);
    while (end > s && g_ascii_isspace(*(end - 1))) *(--end) = '\0';
    return s;
}

static int extrair_ids_de_lista(const char *src, char ids[][8], int max_ids) {
    if (!src || max_ids <= 0) return 0;
    char *copy = g_strdup(src);
    if (!copy) return 0;
    char *s = trim_spaces(copy);
    if (*s == '[') {
        size_t L = strlen(s);
        if (L > 0 && s[L - 1] == ']') { s[L - 1] = '\0'; s++; }
    }
    int count = 0;
    for (char *tok = strtok(s, ","); tok && count < max_ids; tok = strtok(NULL, ",")) {
        tok = trim_spaces(tok);
        size_t lt = strlen(tok);
        if (lt >= 2 && ((tok[0] == '\'' && tok[lt - 1] == '\'') || (tok[0] == '"' && tok[lt - 1] == '"'))) {
            tok[lt - 1] = '\0'; tok++; lt -= 2;
        }
        tok = trim_spaces(tok);
        if (lt >= 7) tok[7] = '\0';
        g_strlcpy(ids[count], tok, 8);
        count++;
    }
    g_free(copy);
    return count;
}

static int existe_flight_id(const char *id, SistemaGestor *gestor) {
    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    if (!voos_manager) return 0;
    return voo_ptr_manager_find_by_id(voos_manager, id) != NULL;
}

static int processar_e_validar_flights(const char *flight_ids_str, SistemaGestor *gestor) {
    if (!flight_ids_str || *flight_ids_str == '\0') return 0;
    char ids[2][8] = {{0}};
    int n = extrair_ids_de_lista(flight_ids_str, ids, 2);
    if (n < 1 || n > 2) return 0;
    for (int i = 0; i < n; i++) {
        if (!validar_flight_id(ids[i]) || !existe_flight_id(ids[i], gestor)) return 0;
    }
    if (n == 2) {
        const Voo *primeiro = voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[0]);
        const Voo *segundo = voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[1]);
        if (!primeiro || !segundo) return 0;
        const char *destino = voo_get_iata_destino(primeiro);
        const char *origem = voo_get_iata_origem(segundo);
        if (!destino || !origem) return 0;
        return strcmp(origem, destino) == 0;
    }
    return 1;
}

void* criar_reserva(char **fields, int n_fields, void *ctx) {
    SistemaGestor *gestor = (SistemaGestor*)ctx;
    if (!gestor || n_fields < 8) return NULL;

    char *reservation_id    = fields[0];
    char *flight_ids        = fields[1];
    char *document_number   = fields[2];
    char *seat              = fields[3];
    char *price_str         = fields[4];
    char *extra_luggage_str = fields[5];
    char *priority_str      = fields[6];
    char *qr_code           = fields[7];

    if (!validar_reservation_id(reservation_id)) return NULL;
    if (!processar_e_validar_flights(flight_ids, gestor)) return NULL;

    if (!document_number || strlen(document_number) == 0 || strlen(document_number) > 9 ||
        !existe_document_number(gestor_global_get_passageiros(gestor), document_number)) {
        return NULL;
    }
    for (const char *p = document_number; *p; ++p) {
        if (!isdigit((unsigned char)*p)) return NULL;
    }

    if (!validar_seat(seat)) return NULL;

    double price;
    if (!validar_preco(price_str, &price)) return NULL;

    bool extra_luggage;
    if (!parse_boolean(extra_luggage_str, &extra_luggage)) return NULL;

    bool priority_boarding;
    if (!parse_boolean(priority_str, &priority_boarding)) return NULL;

    return reserva_create(
        reservation_id,
        flight_ids,
        document_number,
        seat,
        price,
        extra_luggage,
        priority_boarding,
        qr_code
    );
}

ParseResultado* parse_reserva_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    Reserva *res = (Reserva*)criar_reserva(fields, n_fields, gestor);
    if (!res) return r;

    parse_resultado_mark_valid(r, res);
    return r;
}

int csv_parser_reservas(
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
        criar_reserva,
        reserva_ptr_manager_add_from_ptr_adapter,
        separador
    );

    fclose(ficheiro_erros_ptr);
    fclose(ficheiro);

    return contador_validos;
}