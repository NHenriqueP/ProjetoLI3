#include "passageiros.h"
#include <glib.h>

struct Passageiro {
	char *document_number;
	char *first_name;
	char *last_name;
	char *dob;
	char *nationality;
};

Passageiro* passageiro_create(
    const char *document_number,
    const char *first_name,
    const char *last_name,
    const char *dob,
    const char *nationality
) {
    if (!document_number || !first_name || !last_name || !dob ||
        !nationality) {
        return NULL;
    }   
    Passageiro *p = g_new0(Passageiro, 1);
    if (!p) return NULL;
    p->document_number = g_strdup(document_number);
    p->first_name = g_strdup(first_name);
    p->last_name = g_strdup(last_name);
    p->dob = g_strdup(dob);
    p->nationality = g_strdup(nationality);
    return p;   
}

void passageiro_destroy(Passageiro *passageiro) {
    if (!passageiro) return;

    g_free(passageiro->document_number);
    g_free(passageiro->first_name);
    g_free(passageiro->last_name);
    g_free(passageiro->dob);
    g_free(passageiro->nationality);
    g_free(passageiro);
}

const char* get_passageiro_document_number(const Passageiro *passageiro) {
    if (passageiro) return passageiro->document_number;
    return NULL;
}