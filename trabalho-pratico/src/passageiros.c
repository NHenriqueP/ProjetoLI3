#include "passageiros.h"
#include "csv_parser.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
static int validar_document_number(const char *s, size_t len) {
    if (!s || len != 9) return 0; // deve ter exatamente 9 dígitos

    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }

    return s[9] == '\0';
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
ParseResultado parse_passageiro_line(char **fields, int n_fields, SistemaGestor* gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    /* motivo removed to reduce parse result size */
    
    // Esperamos 10 campos: document number, first name, last name, dob, nationality, 
    //                      gender, email, phone, address, photo
    if (n_fields < 10) {
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

    size_t document_number_len = 0;
    size_t first_name_len = 0;
    size_t last_name_len = 0;
    size_t dob_len = 0;
    size_t nationality_len = 0;
    size_t gender_len = 0;
    size_t email_len = 0;
    size_t phone_len = 0;
    size_t address_len = 0;
    
    // Validações
    if (!document_number) {
        return r;
    }

    document_number_len = strlen(document_number);
    if (!validar_document_number(document_number, document_number_len)) {
        return r;
    }
    
    if (!first_name) {
        return r;
    }

    first_name_len = strlen(first_name);
    if (first_name_len == 0) {
        return r;
    }
    
    if (!last_name) {
        return r;
    }

    last_name_len = strlen(last_name);
    if (last_name_len == 0) {
        return r;
    }
    
    if (!dob) {
        return r;
    }

    dob_len = strlen(dob);
    if (!validar_data(dob)) {
        return r;
    }
    
    if (!nationality) {
        return r;
    }

    nationality_len = strlen(nationality);
    if (nationality_len == 0) {
        return r;
    }
    
    if (!gender) {
        return r;
    }

    gender_len = strlen(gender);
    if (!validar_gender(gender)) {
        return r;
    }
    
    if (!email) {
        return r;
    }

    email_len = strlen(email);
    if (!validar_email(email)) {
        return r;
    }
    
    if (!phone) {
        return r;
    }

    phone_len = strlen(phone);
    if (phone_len == 0) {
        return r;
    }
    
    if (!address) {
        return r;
    }

    address_len = strlen(address);
    if (address_len == 0) {
        return r;
    }
    
    // Photo pode ser vazio (opcional)
    
    // Criar passageiro
    Passageiro *p = g_new0(Passageiro, 1);
    if (!p) {
        return r;
    }
    
    p->document_number = (char*)g_memdup2(document_number, document_number_len + 1);
    p->first_name = (char*)g_memdup2(first_name, first_name_len + 1);
    p->last_name = (char*)g_memdup2(last_name, last_name_len + 1);
    p->dob = (char*)g_memdup2(dob, dob_len + 1);
    p->nationality = (char*)g_memdup2(nationality, nationality_len + 1);
    p->gender = (char*)g_memdup2(gender, gender_len + 1);
    p->email = (char*)g_memdup2(email, email_len + 1);
    p->phone = (char*)g_memdup2(phone, phone_len + 1);
    p->address = (char*)g_memdup2(address, address_len + 1);

    size_t photo_len = photo ? strlen(photo) : 0;
    p->photo = (char*)g_memdup2(photo ? photo : "", photo_len + 1);
    
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
        return r;
    }
    
    r.valido = true;
    r.entidade = p;
    return r;
}