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

#endif