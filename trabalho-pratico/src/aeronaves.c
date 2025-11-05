#include "aeronaves.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Definição completa da estrutura Aeronave
typedef struct aeronave {
    char *id;           // identifier
    char *fabricante;   // manufacturer
    char *modelo;       // model
    int ano;            // year
    int capacidade;     // capacity
    int alcance;        // range
} Aeronave;

// Estrutura do gestor com hash table e array
typedef struct AeronaveManager {
    GHashTable *aeronaves_by_id;  // Hash table para busca por ID
    GArray *aeronaves_list;        // Array para iteração
    int count;
} AeronaveManager;

// Variável global para o manager (inicializada como NULL)
static AeronaveManager *mgr = NULL;

// ==================== FUNÇÕES DO MANAGER ====================

// Cria um novo AeronaveManager
AeronaveManager* aeronave_manager_create() {
    AeronaveManager *manager = malloc(sizeof(AeronaveManager));
    if (!manager) return NULL;
    
    manager->count = 0;
    
    // Cria hash table para busca por ID
    manager->aeronaves_by_id = g_hash_table_new(g_str_hash, g_str_equal);
    manager->aeronaves_list = g_array_new(FALSE, FALSE, sizeof(Aeronave*));
    
    if (!manager->aeronaves_by_id || !manager->aeronaves_list) {
        if (manager->aeronaves_by_id) g_hash_table_destroy(manager->aeronaves_by_id);
        if (manager->aeronaves_list) g_array_free(manager->aeronaves_list, TRUE);
        free(manager);
        return NULL;
    }
    
    return manager;
}

// Inicializa o manager global
void init_aeronave_manager() {
    if (!mgr) {
        mgr = aeronave_manager_create();
    }
}

// Obtém o manager global
AeronaveManager* get_aeronave_manager() {
    return mgr;
}

// Adiciona uma aeronave ao manager
int aeronave_manager_add(AeronaveManager *manager, Aeronave aeronave) {
    if (!manager) return 0;
    
    // Aloca memória para uma cópia da aeronave
    Aeronave *new_aeronave = g_new(Aeronave, 1);
    if (!new_aeronave) return 0;
    
    *new_aeronave = aeronave;
    
    // Adiciona à hash table usando ID como chave
    g_hash_table_insert(manager->aeronaves_by_id, new_aeronave->id, new_aeronave);
    
    // Adiciona ao array para iteração
    g_array_append_val(manager->aeronaves_list, new_aeronave);
    
    manager->count++;
    return 1;
}

// Busca uma aeronave por ID (O(1) em média)
Aeronave* aeronave_manager_find_by_id(AeronaveManager *manager, const char *id) {
    if (!manager || !id) return NULL;
    return (Aeronave*)g_hash_table_lookup(manager->aeronaves_by_id, id);
}

// Busca aeronaves por fabricante (retorna primeira encontrada)
Aeronave* aeronave_manager_find_by_manufacturer(AeronaveManager *manager, const char *manufacturer) {
    if (!manager || !manager->aeronaves_list || !manufacturer) return NULL;
    
    for (guint i = 0; i < manager->aeronaves_list->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves_list, Aeronave*, i);
        if (a && a->fabricante && strcmp(a->fabricante, manufacturer) == 0) {
            return a;
        }
    }
    return NULL;
}

// Verifica se uma aeronave existe por ID
int aeronave_exists_by_id(AeronaveManager *manager, const char *id) {
    if (!manager || !id) return 0;
    return g_hash_table_contains(manager->aeronaves_by_id, id);
}

// Obtém o número total de aeronaves
int aeronave_manager_get_count(AeronaveManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

// Obtém uma aeronave por índice (para iteração)
Aeronave* aeronave_manager_get_at_index(AeronaveManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->aeronaves_list, Aeronave*, index);
}

// Liberta a memória de uma aeronave
static void free_aeronave(Aeronave *aeronave) {
    if (!aeronave) return;
    
    g_free(aeronave->id);
    g_free(aeronave->fabricante);
    g_free(aeronave->modelo);
}

// Liberta toda a memória do manager
void aeronave_manager_destroy(AeronaveManager *manager) {
    if (!manager) return;
    
    // Liberta cada aeronave do array
    for (guint i = 0; i < manager->aeronaves_list->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves_list, Aeronave*, i);
        if (a) {
            free_aeronave(a);
            g_free(a);
        }
    }
    
    // Liberta a hash table
    g_hash_table_destroy(manager->aeronaves_by_id);
    g_array_free(manager->aeronaves_list, TRUE);
    
    free(manager);
}

// Liberta o manager global
void cleanup_aeronave_manager() {
    if (mgr) {
        aeronave_manager_destroy(mgr);
        mgr = NULL;
    }
}

// ==================== GETTERS ====================

const char* aeronave_get_id(const Aeronave *a) {
    if (a) return a->id;
    return NULL;
}

const char* aeronave_get_fabricante(const Aeronave *a) {
    if (a) return a->fabricante;
    return NULL;
}

const char* aeronave_get_modelo(const Aeronave *a) {
    if (a) return a->modelo;
    return NULL;
}

int aeronave_get_ano(const Aeronave *a) {
    if (a) return a->ano;
    return -1;
}

int aeronave_get_capacidade(const Aeronave *a) {
    if (a) return a->capacidade;
    return -1;
}

int aeronave_get_alcance(const Aeronave *a) {
    if (a) return a->alcance;
    return -1;
}

// ==================== POINTER-BASED MANAGER ====================

// Cria um gestor baseado em ponteiros
AeronavePtrManager* aeronave_ptr_manager_create() {
    AeronavePtrManager *m = malloc(sizeof(AeronavePtrManager));
    if (!m) return NULL;
    
    m->aeronaves = g_array_new(FALSE, FALSE, sizeof(Aeronave*));
    if (!m->aeronaves) {
        free(m);
        return NULL;
    }
    
    return m;
}

// Destroi o gestor baseado em ponteiros
void aeronave_ptr_manager_destroy(AeronavePtrManager *manager) {
    if (!manager) return;
    
    if (manager->aeronaves) {
        for (guint i = 0; i < manager->aeronaves->len; i++) {
            Aeronave *a = g_array_index(manager->aeronaves, Aeronave*, i);
            if (a) {
                free_aeronave(a);
                g_free(a);
            }
        }
        g_array_free(manager->aeronaves, TRUE);
    }
    
    free(manager);
}

// Adiciona uma aeronave (ponteiro) ao gestor
int aeronave_ptr_manager_add_from_ptr(AeronavePtrManager *manager, Aeronave *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->aeronaves, entidade);
    return 1;
}

// Adapter compatível com csv_parser
int aeronave_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return aeronave_ptr_manager_add_from_ptr((AeronavePtrManager*)gestor, (Aeronave*)entidade);
}

// Busca por ID no gestor de ponteiros
Aeronave* aeronave_ptr_manager_find_by_id(AeronavePtrManager *manager, const char *id) {
    if (!manager || !manager->aeronaves || !id) return NULL;
    
    for (guint i = 0; i < manager->aeronaves->len; i++) {
        Aeronave *a = g_array_index(manager->aeronaves, Aeronave*, i);
        if (a && a->id && strcmp(a->id, id) == 0) return a;
    }
    
    return NULL;
}

// Produz um resumo CSV da aeronave
int aeronave_summary(const Aeronave *aeronave, char *buf, size_t bufsize) {
    if (!aeronave || !buf || bufsize == 0) return 0;
    
    int n = snprintf(buf, bufsize, "%s,%s,%s,%d,%d,%d",
                     aeronave->id ? aeronave->id : "",
                     aeronave->fabricante ? aeronave->fabricante : "",
                     aeronave->modelo ? aeronave->modelo : "",
                     aeronave->ano,
                     aeronave->capacidade,
                     aeronave->alcance);
    
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

// ==================== CSV PARSER ====================

// Valida se o identifier é válido (alfanumérico com possíveis hífens)
static int validar_identifier(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    
    for (size_t i = 0; s[i] != '\0'; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != '-') return 0;
    }
    return 1;
}

// Valida se é um número positivo
static int validar_numero_positivo(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    
    int num = atoi(s);
    return num > 0;
}

// Parser de linha CSV para aeronaves
ParseResultado parse_aeronave_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    r.motivo = NULL;
    
    // Esperamos 6 campos: identifier, manufacturer, model, year, capacity, range
    if (n_fields < 6) {
        r.motivo = "Número incorreto de campos";
        return r;
    }
    
    char *identifier   = fields[0];
    char *manufacturer = fields[1];
    char *model        = fields[2];
    char *year_str     = fields[3];
    char *capacity_str = fields[4];
    char *range_str    = fields[5];
    
    // Validações
    if (!validar_identifier(identifier)) {
        r.motivo = "Identifier inválido";
        return r;
    }
    
    if (!manufacturer || strlen(manufacturer) == 0) {
        r.motivo = "Manufacturer vazio";
        return r;
    }
    
    if (!model || strlen(model) == 0) {
        r.motivo = "Model vazio";
        return r;
    }
    
    if (!validar_numero_positivo(year_str)) {
        r.motivo = "Year inválido";
        return r;
    }
    
    if (!validar_numero_positivo(capacity_str)) {
        r.motivo = "Capacity inválido";
        return r;
    }
    
    if (!validar_numero_positivo(range_str)) {
        r.motivo = "Range inválido";
        return r;
    }
    
    // Converter strings para inteiros
    int year = atoi(year_str);
    int capacity = atoi(capacity_str);
    int range = atoi(range_str);
    
    // Validação de ano (entre 1900 e 2100)
    if (year < 1900 || year > 2100) {
        r.motivo = "Ano fora do intervalo válido";
        return r;
    }
    
    // Criar aeronave
    Aeronave *a = g_new0(Aeronave, 1);
    if (!a) {
        r.motivo = "Falha de memória ao criar aeronave";
        return r;
    }
    
    a->id = g_strdup(identifier);
    a->fabricante = g_strdup(manufacturer);
    a->modelo = g_strdup(model);
    a->ano = year;
    a->capacidade = capacity;
    a->alcance = range;
    
    if (!a->id || !a->fabricante || !a->modelo) {
        g_free(a->id);
        g_free(a->fabricante);
        g_free(a->modelo);
        g_free(a);
        r.motivo = "Erro de alocação de memória";
        return r;
    }
    
    r.valido = true;
    r.entidade = a;
    return r;
}