#ifndef RESERVAS
#define RESERVAS

#include <stdbool.h>

typedef struct Reserva Reserva;

Reserva* reserva_create(const char *reservation_id,
                        const char *flight_ids,
                        const char *document_number,
                        const char *seat,
                        double price,
                        bool extra_luggage,
                        bool priority_boarding,
                        const char *qr_code);

void reserva_destroy(Reserva *reserva);

const char* reserva_get_reservation_id(const Reserva *reserva);

// Devolve a string com a lista de flight ids tal como vem no CSV (ex.: "['AB12345', 'CD67890']")
const char* reserva_get_flight_ids(const Reserva *reserva);

// Documento do passageiro associado à reserva
const char* reserva_get_document_number(const Reserva *reserva);

#endif