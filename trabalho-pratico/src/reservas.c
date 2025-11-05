#include "reservas.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Definição completa da estrutura Reserva
typedef struct reserva {
    char *reservation_id;      // reservation id (ex: R000019411)
    char *flight_ids;          // flight ids (ex: ['TI12433', 'TI12434'])
    char *document_number;     // document number
    char *seat;                // seat (ex: 3C)
    double price;              // price (ex: 163.84)
    bool extra_luggage;        // extra luggage (true/false)
    bool priority_boarding;    // priority boarding (true/false)
    char *qr_code;             // qr code (base64 string)
} Reserva;

// Estrutura do gestor com hash table e array
typedef struct ReservaManager {
    GHashTable *reservas_by_id;  // Hash table para busca por reservation id
    GArray *reservas_list;        // Array para iteração
    int count;
} ReservaManager;

// Variável global para o manager (inicializada como NULL)
static ReservaManager *mgr = NULL;

// ==================== FUNÇÕES DO MANAGER ====================

// Cria um novo ReservaManager
ReservaManager* reserva_manager_create() {
    ReservaManager *manager = malloc(sizeof(ReservaManager));
    if (!manager) return NULL;
    
    manager->count = 0;
    
    // Cria hash table para busca por reservation id
    manager->reservas_by_id = g_hash_table_new(g_str_hash, g_str_equal);
    manager->reservas_list = g_array_new(FALSE, FALSE, sizeof(Reserva*));
    
    if (!manager->reservas_by_id || !manager->reservas_list) {
        if (manager->reservas_by_id) g_hash_table_destroy(manager->reservas_by_id);
        if (manager->reservas_list) g_array_free(manager->reservas_list, TRUE);
        free(manager);
        return NULL;
    }
    
    return manager;
}

// Inicializa o manager global
void init_reserva_manager() {
    if (!mgr) {
        mgr = reserva_manager_create();
    }
}

// Obtém o manager global
ReservaManager* get_reserva_manager() {
    return mgr;
}

// Adiciona uma reserva ao manager
int reserva_manager_add(ReservaManager *manager, Reserva reserva) {
    if (!manager) return 0;
    
    // Aloca memória para uma cópia da reserva
    Reserva *new_reserva = g_new(Reserva, 1);
    if (!new_reserva) return 0;
    
    *new_reserva = reserva;
    
    // Adiciona à hash table usando reservation id como chave
    g_hash_table_insert(manager->reservas_by_id, new_reserva->reservation_id, new_reserva);
    
    // Adiciona ao array para iteração
    g_array_append_val(manager->reservas_list, new_reserva);
    
    manager->count++;
    return 1;
}

// Busca uma reserva por ID (O(1) em média)
Reserva* reserva_manager_find_by_id(ReservaManager *manager, const char *reservation_id) {
    if (!manager || !reservation_id) return NULL;
    return (Reserva*)g_hash_table_lookup(manager->reservas_by_id, reservation_id);
}

// Busca reserva por document number (busca linear)
Reserva* reserva_manager_find_by_document(ReservaManager *manager, const char *document_number) {
    if (!manager || !manager->reservas_list || !document_number) return NULL;
    
    for (guint i = 0; i < manager->reservas_list->len; i++) {
        Reserva *r = g_array_index(manager->reservas_list, Reserva*, i);
        if (r && r->document_number && strcmp(r->document_number, document_number) == 0) {
            return r;
        }
    }
    return NULL;
}

// Verifica se uma reserva existe por ID
int reserva_exists_by_id(ReservaManager *manager, const char *reservation_id) {
    if (!manager || !reservation_id) return 0;
    return g_hash_table_contains(manager->reservas_by_id, reservation_id);
}

// Obtém o número total de reservas
int reserva_manager_get_count(ReservaManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

// Obtém uma reserva por índice (para iteração)
Reserva* reserva_manager_get_at_index(ReservaManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->reservas_list, Reserva*, index);
}

// Liberta a memória de uma reserva
static void free_reserva(Reserva *reserva) {
    if (!reserva) return;
    
    g_free(reserva->reservation_id);
    g_free(reserva->flight_ids);
    g_free(reserva->document_number);
    g_free(reserva->seat);
    g_free(reserva->qr_code);
}

// Liberta toda a memória do manager
void reserva_manager_destroy(ReservaManager *manager) {
    if (!manager) return;
    
    // Liberta cada reserva do array
    for (guint i = 0; i < manager->reservas_list->len; i++) {
        Reserva *r = g_array_index(manager->reservas_list, Reserva*, i);
        if (r) {
            free_reserva(r);
            g_free(r);
        }
    }
    
    // Liberta a hash table
    g_hash_table_destroy(manager->reservas_by_id);
    g_array_free(manager->reservas_list, TRUE);
    
    free(manager);
}

// Liberta o manager global
void cleanup_reserva_manager() {
    if (mgr) {
        reserva_manager_destroy(mgr);
        mgr = NULL;
    }
}

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

// ==================== POINTER-BASED MANAGER ====================

// Cria um gestor baseado em ponteiros
ReservaPtrManager* reserva_ptr_manager_create() {
    ReservaPtrManager *m = malloc(sizeof(ReservaPtrManager));
    if (!m) return NULL;
    
    m->reservas = g_array_new(FALSE, FALSE, sizeof(Reserva*));
    if (!m->reservas) {
        free(m);
        return NULL;
    }
    
    return m;
}

// Destroi o gestor baseado em ponteiros
void reserva_ptr_manager_destroy(ReservaPtrManager *manager) {
    if (!manager) return;
    
    if (manager->reservas) {
        for (guint i = 0; i < manager->reservas->len; i++) {
            Reserva *r = g_array_index(manager->reservas, Reserva*, i);
            if (r) {
                free_reserva(r);
                g_free(r);
            }
        }
        g_array_free(manager->reservas, TRUE);
    }
    
    free(manager);
}

// Adiciona uma reserva (ponteiro) ao gestor
int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->reservas, entidade);
    return 1;
}

// Adapter compatível com csv_parser
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return reserva_ptr_manager_add_from_ptr((ReservaPtrManager*)gestor, (Reserva*)entidade);
}

// Busca por ID no gestor de ponteiros
Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id) {
    if (!manager || !manager->reservas || !reservation_id) return NULL;
    
    for (guint i = 0; i < manager->reservas->len; i++) {
        Reserva *r = g_array_index(manager->reservas, Reserva*, i);
        if (r && r->reservation_id && strcmp(r->reservation_id, reservation_id) == 0) return r;
    }
    
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
    if (!s || strlen(s) < 2) return 0;
    if (s[0] != 'R') return 0;
    
    for (size_t i = 1; s[i] != '\0'; i++) {
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
    
    if (strcmp(s, "true") == 0 || strcmp(s, "1") == 0) {
        *out = true;
        return 1;
    }
    if (strcmp(s, "false") == 0 || strcmp(s, "0") == 0) {
        *out = false;
        return 1;
    }
    
    return 0;
}

// Parser de linha CSV para reservas
ParseResultado parse_reserva_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    r.motivo = NULL;
    
    // Esperamos 8 campos: reservation id, flight ids, document number, seat, 
    //                     price, extra luggage, priority boarding, qr code
    if (n_fields < 8) {
        r.motivo = "Número incorreto de campos";
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
        r.motivo = "Reservation ID inválido";
        return r;
    }
    
    if (!flight_ids || strlen(flight_ids) == 0) {
        r.motivo = "Flight IDs vazio";
        return r;
    }
    
    if (!document_number || strlen(document_number) == 0) {
        r.motivo = "Document number vazio";
        return r;
    }
    
    if (!validar_seat(seat)) {
        r.motivo = "Seat inválido";
        return r;
    }
    
    double price;
    if (!validar_preco(price_str, &price)) {
        r.motivo = "Price inválido";
        return r;
    }
    
    bool extra_luggage;
    if (!parse_boolean(extra_luggage_str, &extra_luggage)) {
        r.motivo = "Extra luggage inválido (deve ser true/false)";
        return r;
    }
    
    bool priority_boarding;
    if (!parse_boolean(priority_str, &priority_boarding)) {
        r.motivo = "Priority boarding inválido (deve ser true/false)";
        return r;
    }
    
    // QR code pode ser vazio (opcional)
    
    // Criar reserva
    Reserva *res = g_new0(Reserva, 1);
    if (!res) {
        r.motivo = "Falha de memória ao criar reserva";
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
        r.motivo = "Erro de alocação de memória";
        return r;
    }
    
    r.valido = true;
    r.entidade = res;
    return r;
}