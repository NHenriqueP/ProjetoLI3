#include "passageiros.h"
#include <stdio.h>

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

// Parser movido para parser_passageiro.c