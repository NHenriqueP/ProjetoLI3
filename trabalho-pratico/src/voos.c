#include "voos.h"
#include <ctype.h>
#include <string.h>

Status voo_get_status(const Voo *voo) {
	return voo ? voo->estado : ON_TIME;
}

const char* voo_get_aircraft(const Voo *voo) {
    return voo ? voo->aircraft : NULL;
}

const char* voo_get_iata_origem(const Voo *voo) {
    return voo ? voo->iataorigem : NULL;
}

const char* voo_get_iata_destino(const Voo *voo) {
    return voo ? voo->iatadestino : NULL;
}

const Horario* voo_get_actual_departure(const Voo *voo) {
	return voo ? &voo->actualdeparture : NULL;
}

int validar_flight_id(const char *s) {
	if (!s || strlen(s) != 7) return 0;
	
	for (int j = 0; j < 2; j++) {
		if (!isupper((unsigned char)s[j])) return 0;
	}
	for (int i = 2; i < 7; i++) {
		if (!isdigit((unsigned char)s[i])) return 0;
	}
	return 1;

}
// Parser movido para parser_voo.c
