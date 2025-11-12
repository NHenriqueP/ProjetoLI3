#ifndef PASSAGEIRO
#define PASSAGEIRO

#include <stddef.h>

typedef struct passageiro {
	char *document_number;
	char *first_name;
	char *last_name;
	char *dob;
	char *nationality;
	char *gender;
	char *email;
	char *phone;
	char *address;
	char *photo;
} Passageiro;

int passageiro_summary(const Passageiro *passageiro, char *buf, size_t bufsize);

const char* passageiro_get_document_number(const Passageiro *p);
const char* passageiro_get_first_name(const Passageiro *p);
const char* passageiro_get_last_name(const Passageiro *p);
const char* passageiro_get_dob(const Passageiro *p);
const char* passageiro_get_nationality(const Passageiro *p);
const char* passageiro_get_gender(const Passageiro *p);
const char* passageiro_get_email(const Passageiro *p);
const char* passageiro_get_phone(const Passageiro *p);
const char* passageiro_get_address(const Passageiro *p);
const char* passageiro_get_photo(const Passageiro *p);

#endif