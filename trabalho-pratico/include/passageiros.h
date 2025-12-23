#ifndef PASSAGEIRO
#define PASSAGEIRO

#include <stddef.h>

typedef struct Passageiro Passageiro;

Passageiro* passageiro_create(
	const char *document_number,
	const char *first_name,
	const char *last_name,
	const char *dob,
	const char *nationality
);
void passageiro_destroy(Passageiro *passageiro);

const char* get_passageiro_document_number(const Passageiro *passageiro);

const char* passageiro_get_first_name(const Passageiro *passageiro);
const char* passageiro_get_last_name(const Passageiro *passageiro);
const char* passageiro_get_dob(const Passageiro *passageiro);
const char* passageiro_get_nationality(const Passageiro *passageiro);

#endif