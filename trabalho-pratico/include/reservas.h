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

#endif