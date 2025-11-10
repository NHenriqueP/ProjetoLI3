#include "passageiros.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Definição completa da estrutura Passageiro
typedef struct passageiro {
    char *document_number;  // document number
    char *first_name;       // first name
    char *last_name;        // last name
    char *dob;              // date of birth (YYYY-MM-DD)
    char *nationality;      // nationality
    char *gender;           // gender (M/F)
    char *email;            // email
    char *phone;            // phone
    char *address;          // address
    char *photo;            // photo (base64 string)
} Passageiro;

// Estrutura do gestor com hash table e array
typedef struct PassageiroManager {
    GHashTable *passageiros_by_doc;  // Hash table para busca por document number
    GArray *passageiros_list;         // Array para iteração
    int count;
} PassageiroManager;

// Variável global para o manager (inicializada como NULL)
static PassageiroManager *mgr = NULL;

// ==================== FUNÇÕES DO MANAGER ====================

// Cria um novo PassageiroManager
PassageiroManager* passageiro_manager_create() {
    PassageiroManager *manager = malloc(sizeof(PassageiroManager));
    if (!manager) return NULL;
    
    manager->count = 0;
    
    // Cria hash table para busca por document number
    manager->passageiros_by_doc = g_hash_table_new(g_str_hash, g_str_equal);
    manager->passageiros_list = g_array_new(FALSE, FALSE, sizeof(Passageiro*));
    
    if (!manager->passageiros_by_doc || !manager->passageiros_list) {
        if (manager->passageiros_by_doc) g_hash_table_destroy(manager->passageiros_by_doc);
        if (manager->passageiros_list) g_array_free(manager->passageiros_list, TRUE);
        free(manager);
        return NULL;
    }
    
    return manager;
}

// Inicializa o manager global
void init_passageiro_manager() {
    if (!mgr) {
        mgr = passageiro_manager_create();
    }
}

// Obtém o manager global
PassageiroManager* get_passageiro_manager() {
    return mgr;
}

// Adiciona um passageiro ao manager
int passageiro_manager_add(PassageiroManager *manager, Passageiro passageiro) {
    if (!manager) return 0;
    
    // Aloca memória para uma cópia do passageiro
    Passageiro *new_passageiro = g_new(Passageiro, 1);
    if (!new_passageiro) return 0;
    
    *new_passageiro = passageiro;
    
    // Adiciona à hash table usando document number como chave
    g_hash_table_insert(manager->passageiros_by_doc, new_passageiro->document_number, new_passageiro);
    
    // Adiciona ao array para iteração
    g_array_append_val(manager->passageiros_list, new_passageiro);
    
    manager->count++;
    return 1;
}

// Busca um passageiro por document number (O(1) em média)
Passageiro* passageiro_manager_find_by_doc(PassageiroManager *manager, const char *document_number) {
    if (!manager || !document_number) return NULL;
    return (Passageiro*)g_hash_table_lookup(manager->passageiros_by_doc, document_number);
}

// Busca passageiro por email (busca linear)
Passageiro* passageiro_manager_find_by_email(PassageiroManager *manager, const char *email) {
    if (!manager || !manager->passageiros_list || !email) return NULL;
    
    for (guint i = 0; i < manager->passageiros_list->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros_list, Passageiro*, i);
        if (p && p->email && strcmp(p->email, email) == 0) {
            return p;
        }
    }
    return NULL;
}

// Verifica se um passageiro existe por document number
int passageiro_exists_by_doc(PassageiroManager *manager, const char *document_number) {
    if (!manager || !document_number) return 0;
    return g_hash_table_contains(manager->passageiros_by_doc, document_number);
}

// Obtém o número total de passageiros
int passageiro_manager_get_count(PassageiroManager *manager) {
    if (!manager) return 0;
    return manager->count;
}

// Obtém um passageiro por índice (para iteração)
Passageiro* passageiro_manager_get_at_index(PassageiroManager *manager, int index) {
    if (!manager || index < 0 || index >= manager->count) return NULL;
    return g_array_index(manager->passageiros_list, Passageiro*, index);
}

// Liberta a memória de um passageiro
static void free_passageiro(Passageiro *passageiro) {
    if (!passageiro) return;
    
    g_free(passageiro->document_number);
    g_free(passageiro->first_name);
    g_free(passageiro->last_name);
    g_free(passageiro->dob);
    g_free(passageiro->nationality);
    g_free(passageiro->gender);
    g_free(passageiro->email);
    g_free(passageiro->phone);
    g_free(passageiro->address);
    g_free(passageiro->photo);
}

// Liberta toda a memória do manager
void passageiro_manager_destroy(PassageiroManager *manager) {
    if (!manager) return;
    
    // Liberta cada passageiro do array
    for (guint i = 0; i < manager->passageiros_list->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros_list, Passageiro*, i);
        if (p) {
            free_passageiro(p);
            g_free(p);
        }
    }
    
    // Liberta a hash table
    g_hash_table_destroy(manager->passageiros_by_doc);
    g_array_free(manager->passageiros_list, TRUE);
    
    free(manager);
}

// Liberta o manager global
void cleanup_passageiro_manager() {
    if (mgr) {
        passageiro_manager_destroy(mgr);
        mgr = NULL;
    }
}

// ==================== GETTERS ====================

const char* passageiro_get_document_number(const Passageiro *p) {
    if (p) return p->document_number;
    return NULL;
}

const char* passageiro_get_first_name(const Passageiro *p) {
    if (p) return p->first_name;
    return NULL;
}

const char* passageiro_get_last_name(const Passageiro *p) {
    if (p) return p->last_name;
    return NULL;
}

const char* passageiro_get_dob(const Passageiro *p) {
    if (p) return p->dob;
    return NULL;
}

const char* passageiro_get_nationality(const Passageiro *p) {
    if (p) return p->nationality;
    return NULL;
}

const char* passageiro_get_gender(const Passageiro *p) {
    if (p) return p->gender;
    return NULL;
}

const char* passageiro_get_email(const Passageiro *p) {
    if (p) return p->email;
    return NULL;
}

const char* passageiro_get_phone(const Passageiro *p) {
    if (p) return p->phone;
    return NULL;
}

const char* passageiro_get_address(const Passageiro *p) {
    if (p) return p->address;
    return NULL;
}

const char* passageiro_get_photo(const Passageiro *p) {
    if (p) return p->photo;
    return NULL;
}

// ==================== POINTER-BASED MANAGER ====================

// Cria um gestor baseado em ponteiros
PassageiroPtrManager* passageiro_ptr_manager_create() {
    PassageiroPtrManager *m = malloc(sizeof(PassageiroPtrManager));
    if (!m) return NULL;
    
    m->passageiros = g_array_new(FALSE, FALSE, sizeof(Passageiro*));
    if (!m->passageiros) {
        free(m);
        return NULL;
    }
    
    return m;
}

// Destroi o gestor baseado em ponteiros
void passageiro_ptr_manager_destroy(PassageiroPtrManager *manager) {
    if (!manager) return;
    
    if (manager->passageiros) {
        for (guint i = 0; i < manager->passageiros->len; i++) {
            Passageiro *p = g_array_index(manager->passageiros, Passageiro*, i);
            if (p) {
                free_passageiro(p);
                g_free(p);
            }
        }
        g_array_free(manager->passageiros, TRUE);
    }
    
    free(manager);
}

// Adiciona um passageiro (ponteiro) ao gestor
int passageiro_ptr_manager_add_from_ptr(PassageiroPtrManager *manager, Passageiro *entidade) {
    if (!manager || !entidade) return 0;
    g_array_append_val(manager->passageiros, entidade);
    return 1;
}

// Adapter compatível com csv_parser
int passageiro_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade) {
    if (!gestor || !entidade) return 0;
    return passageiro_ptr_manager_add_from_ptr((PassageiroPtrManager*)gestor, (Passageiro*)entidade);
}

// Busca por document number no gestor de ponteiros
Passageiro* passageiro_ptr_manager_find_by_doc(PassageiroPtrManager *manager, const char *document_number) {
    if (!manager || !manager->passageiros || !document_number) return NULL;
    
    for (guint i = 0; i < manager->passageiros->len; i++) {
        Passageiro *p = g_array_index(manager->passageiros, Passageiro*, i);
        if (p && p->document_number && strcmp(p->document_number, document_number) == 0) return p;
    }
    
    return NULL;
}

// Produz um resumo CSV do passageiro
int passageiro_summary(const Passageiro *passageiro, char *buf, size_t bufsize) {
    if (!passageiro || !buf || bufsize == 0) return 0;
    
    int n = snprintf(buf, bufsize, "%s,%s,%s,%s,%s,%s,%s,%s,%s",
                     passageiro->document_number ? passageiro->document_number : "",
                     passageiro->first_name ? passageiro->first_name : "",
                     passageiro->last_name ? passageiro->last_name : "",
                     passageiro->dob ? passageiro->dob : "",
                     passageiro->nationality ? passageiro->nationality : "",
                     passageiro->gender ? passageiro->gender : "",
                     passageiro->email ? passageiro->email : "",
                     passageiro->phone ? passageiro->phone : "",
                     passageiro->address ? passageiro->address : "");
    
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

// ==================== CSV PARSER ====================

// Valida se o document number é válido (apenas dígitos)
static int validar_document_number(const char *s) {
    if (!s || strlen(s) != 9) return 0; // Exemplo: deve ter exatamente 9 dígitos

    char *endp;
    strtol(s, &endp, 10); // tenta converter
    if (*endp != '\0') return 0; // se não consumiu toda a string, é inválido

    return 1;
}

// Valida data no formato YYYY-MM-DD
static int validar_data(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    
    // Formato: YYYY-MM-DD
    if (s[4] != '-' || s[7] != '-') return 0;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    
    // Validação básica de ano, mês, dia
    int year = (s[0]-'0')*1000 + (s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0');
    int month = (s[5]-'0')*10 + (s[6]-'0');
    int day = (s[8]-'0')*10 + (s[9]-'0');
    
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    return 1;
}

// Valida gender (M ou F)
static int validar_gender(const char *s) {
    if (!s || strlen(s) != 1) return 0;
    return (s[0] == 'M' || s[0] == 'F'|| s[0]=='O');
}

// Valida email (contém @ e .)
static int validar_email(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    
    int tem_arroba = 0;
    int tem_ponto_depois_arroba = 0;
    
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '@') {
            tem_arroba = 1;
        }
        if (tem_arroba && s[i] == '.') {
            tem_ponto_depois_arroba = 1;
        }
    }
    
    return tem_arroba && tem_ponto_depois_arroba;
}

// Parser de linha CSV para passageiros
ParseResultado parse_passageiro_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    r.motivo = NULL;
    
    // Esperamos 10 campos: document number, first name, last name, dob, nationality, 
    //                      gender, email, phone, address, photo
    if (n_fields < 10) {
        r.motivo = "Número incorreto de campos";
        return r;
    }
    
    char *document_number = fields[0];
    char *first_name      = fields[1];
    char *last_name       = fields[2];
    char *dob             = fields[3];
    char *nationality     = fields[4];
    char *gender          = fields[5];
    char *email           = fields[6];
    char *phone           = fields[7];
    char *address         = fields[8];
    char *photo           = fields[9];
    
    // Validações
    if (!validar_document_number(document_number)) {
        r.motivo = "Document number inválido";
        return r;
    }
    
    if (!first_name || strlen(first_name) == 0) {
        r.motivo = "First name vazio";
        return r;
    }
    
    if (!last_name || strlen(last_name) == 0) {
        r.motivo = "Last name vazio";
        return r;
    }
    
    if (!validar_data(dob)) {
        r.motivo = "Data de nascimento inválida";
        return r;
    }
    
    if (!nationality || strlen(nationality) == 0) {
        r.motivo = "Nationality vazio";
        return r;
    }
    
    if (!validar_gender(gender)) {
        r.motivo = "Gender inválido (deve ser M ou F)";
        return r;
    }
    
    if (!validar_email(email)) {
        r.motivo = "Email inválido";
        return r;
    }
    
    if (!phone || strlen(phone) == 0) {
        r.motivo = "Phone vazio";
        return r;
    }
    
    if (!address || strlen(address) == 0) {
        r.motivo = "Address vazio";
        return r;
    }
    
    // Photo pode ser vazio (opcional)
    
    // Criar passageiro
    Passageiro *p = g_new0(Passageiro, 1);
    if (!p) {
        r.motivo = "Falha de memória ao criar passageiro";
        return r;
    }
    
    p->document_number = g_strdup(document_number);
    p->first_name = g_strdup(first_name);
    p->last_name = g_strdup(last_name);
    p->dob = g_strdup(dob);
    p->nationality = g_strdup(nationality);
    p->gender = g_strdup(gender);
    p->email = g_strdup(email);
    p->phone = g_strdup(phone);
    p->address = g_strdup(address);
    p->photo = g_strdup(photo ? photo : "");
    
    if (!p->document_number || !p->first_name || !p->last_name || !p->dob ||
        !p->nationality || !p->gender || !p->email || !p->phone || !p->address || !p->photo) {
        g_free(p->document_number);
        g_free(p->first_name);
        g_free(p->last_name);
        g_free(p->dob);
        g_free(p->nationality);
        g_free(p->gender);
        g_free(p->email);
        g_free(p->phone);
        g_free(p->address);
        g_free(p->photo);
        g_free(p);
        r.motivo = "Erro de alocação de memória";
        return r;
    }
    
    r.valido = true;
    r.entidade = p;
    return r;
}