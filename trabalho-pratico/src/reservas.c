#include "reservas.h"
#include "csv_parser.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "gestorglobal.h"
#include "voos.h"

// ==================== GETTERS ====================

const char* reserva_get_id(const Reserva *r) {
    if (r) return r->reservation_id;
    return NULL;
}

const char* reserva_get_flight_ids(const Reserva *r) {
    if (r) return r->flight_ids;
    return NULL;
}

const char* reserva_get_document_number(const Reserva *r) {
    if (r) return r->document_number;
    return NULL;
}

const char* reserva_get_seat(const Reserva *r) {
    if (r) return r->seat;
    return NULL;
}

double reserva_get_price(const Reserva *r) {
    if (r) return r->price;
    return -1.0;
}

bool reserva_get_extra_luggage(const Reserva *r) {
    if (r) return r->extra_luggage;
    return false;
}

bool reserva_get_priority_boarding(const Reserva *r) {
    if (r) return r->priority_boarding;
    return false;
}

const char* reserva_get_qr_code(const Reserva *r) {
    if (r) return r->qr_code;
    return NULL;
}

// Produz um resumo CSV da reserva
int reserva_summary(const Reserva *reserva, char *buf, size_t bufsize) {
    if (!reserva || !buf || bufsize == 0) return 0;
    
    int n = snprintf(buf, bufsize, "%s,%s,%s,%s,%.2f,%s,%s",
                     reserva->reservation_id ? reserva->reservation_id : "",
                     reserva->flight_ids ? reserva->flight_ids : "",
                     reserva->document_number ? reserva->document_number : "",
                     reserva->seat ? reserva->seat : "",
                     reserva->price,
                     reserva->extra_luggage ? "true" : "false",
                     reserva->priority_boarding ? "true" : "false");
    
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

// ==================== CSV PARSER ====================

// Valida se o reservation ID é válido (formato R + dígitos)
static int validar_reservation_id(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    size_t len = strlen(s);

    if (s[0] != 'R') return 0;

    for (size_t i = 1; i < len; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

// Valida se o seat é válido (número + letra, ex: 3C, 12A)
static int validar_seat(const char *s) {
    if (!s || strlen(s) < 2) return 0;
    
    // Deve começar com dígitos
    int i = 0;
    if (!isdigit((unsigned char)s[i])) return 0;
    
    while (s[i] != '\0' && isdigit((unsigned char)s[i])) {
        i++;
    }
    
    // Depois deve ter uma letra
    if (s[i] == '\0' || !isalpha((unsigned char)s[i])) return 0;
    i++;
    
    // Não deve ter mais nada depois
    if (s[i] != '\0') return 0;
    
    return 1;
}

// Valida preço (número positivo com até 2 casas decimais)
static int validar_preco(const char *s, double *out) {
    if (!s || !out) return 0;
    
    char *endptr;
    double val = strtod(s, &endptr);
    
    if (endptr == s || *endptr != '\0') return 0;
    if (val < 0) return 0;
    
    *out = val;
    return 1;
}

// Converte string para booleano
static int parse_boolean(const char *s, bool *out) {
    if (!s || !out) return 0;
    if (g_ascii_strcasecmp(s, "true") == 0 || strcmp(s, "1") == 0) { *out = true; return 1; }
    if (g_ascii_strcasecmp(s, "false") == 0 || strcmp(s, "0") == 0) { *out = false; return 1; }
    return 0;
}

int existe_flight_id(const char* id, SistemaGestor* gestor) {
    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    
    if (!voos_manager) return 0;

    Voo* voo = voo_ptr_manager_find_by_id(voos_manager, id);
    return voo != NULL;
 }

static char* trim_spaces(char *s) {
    if (!s) return s;
    while (g_ascii_isspace(*s)) s++;
    char *end = s + strlen(s);
    while (end > s && g_ascii_isspace(*(end-1))) *(--end) = '\0';
    return s;
}

static int extrair_ids_de_lista(const char *src, char ids[][8], int max_ids) {
    if (!src || max_ids <= 0) return 0;
    char *copy = g_strdup(src);
    if (!copy) return 0;
    char *s = trim_spaces(copy);
    if (*s == '[') {
        size_t L = strlen(s);
        if (L > 0 && s[L-1] == ']') { s[L-1] = '\0'; s++; }
    }
    int count = 0;
    for (char *tok = strtok(s, ","); tok && count < max_ids; tok = strtok(NULL, ",")) {
        tok = trim_spaces(tok);
        size_t lt = strlen(tok);
        if (lt >= 2 && ((tok[0]=='\'' && tok[lt-1]=='\'') || (tok[0]=='"' && tok[lt-1]=='"'))) {
            tok[lt-1] = '\0'; tok++; lt -= 2;
        }
        tok = trim_spaces(tok);
        if (lt >= 7) tok[7] = '\0';
        g_strlcpy(ids[count], tok, 8);
        count++;
    }
    g_free(copy);
    return count;
}

int processar_e_validar_flights(const char* flight_ids_str, SistemaGestor* gestor) {
    if (!flight_ids_str || *flight_ids_str == '\0') return 0;
    char ids[2][8] = {{0}};
    int n = extrair_ids_de_lista(flight_ids_str, ids, 2);
    if (n < 1 || n > 2) return 0;
    for (int i = 0; i < n; i++) {
        if (!validar_flight_id(ids[i]) || !existe_flight_id(ids[i], gestor)) return 0;
    }

    if (n==2) {
       char *destino = voo_get_iata_destino(voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[0]));
       char *origem = voo_get_iata_origem(voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[1]));

       return (strcmp(origem, destino) == 0);
    }
    return 1;
}
 


// Parser de linha CSV para reservas
ParseResultado parse_reserva_line(char **fields, int n_fields, SistemaGestor* gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    
    // Esperamos 8 campos: reservation id, flight ids, document number, seat, 
    //                     price, extra luggage, priority boarding, qr code
    if (n_fields < 8) {
        return r;
    }
    
    char *reservation_id    = fields[0];
    char *flight_ids        = fields[1];
    char *document_number   = fields[2];
    char *seat              = fields[3];
    char *price_str         = fields[4];
    char *extra_luggage_str = fields[5];
    char *priority_str      = fields[6];
    char *qr_code           = fields[7];
    
    // Validações
    if (!validar_reservation_id(reservation_id)) {
        return r;
    }
    
    if (!processar_e_validar_flights(flight_ids, gestor)) {
        return r;
    }
    
    if (!document_number || strlen(document_number) == 0 || strlen(document_number) > 9) {
        return r;
    }
    for (const char *p = document_number; *p; ++p) {
        if (!isdigit((unsigned char)*p)) { return r; }
    }
    
    if (!validar_seat(seat)) {
        return r;
    }
    
    double price;
    if (!validar_preco(price_str, &price)) {
        return r;
    }
    
    bool extra_luggage;
    if (!parse_boolean(extra_luggage_str, &extra_luggage)) {
        return r;
    }
    
    bool priority_boarding;
    if (!parse_boolean(priority_str, &priority_boarding)) {
        return r;
    }
    
    // QR code pode ser vazio (opcional)
    
    // Criar reserva
    Reserva *res = g_new0(Reserva, 1);
    if (!res) {
        return r;
    }
    
    res->reservation_id = g_strdup(reservation_id);
    res->flight_ids = g_strdup(flight_ids);
    res->document_number = g_strdup(document_number);
    res->seat = g_strdup(seat);
    res->price = price;
    res->extra_luggage = extra_luggage;
    res->priority_boarding = priority_boarding;
    res->qr_code = g_strdup(qr_code ? qr_code : "");
    
    if (!res->reservation_id || !res->flight_ids || !res->document_number || 
        !res->seat || !res->qr_code) {
        g_free(res->reservation_id);
        g_free(res->flight_ids);
        g_free(res->document_number);
        g_free(res->seat);
        g_free(res->qr_code);
        g_free(res);
        return r;
    }
    
    r.valido = true;
    r.entidade = res;
    return r;
}