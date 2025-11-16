#ifndef PASSAGEIRO
#define PASSAGEIRO

#include <stddef.h>

typedef struct Passageiro Passageiro;

Passageiro* passageiro_create(
	const char *document_number,
	const char *first_name,
	const char *last_name,
	const char *dob,
	const char *nationality,
	const char *gender,	
	const char *email,
	const char *phone,
	const char *address,
	const char *photo
);
void passageiro_destroy(Passageiro *passageiro);

const char* get_passageiro_document_number(const Passageiro *passageiro);

#endif