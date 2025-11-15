#ifndef VOO
#define VOO

#include <stddef.h>

typedef struct date {
    int year, month, day;
} Date;

typedef struct hora {
    int hora, minuto;
} Hora;

typedef struct horario {
    Date data;
    Hora hora;
} Horario;

typedef enum {
    ON_TIME,
    DELAYED,
    CANCELLED
} Status;

typedef struct Voo Voo;

void voo_destroy(Voo *voo);
Voo* voo_create(const char *idvoo,
                const Horario *departure,
                const Horario *actualdeparture,
                const Horario *arrival,
                const Horario *actualarrival,
                int gate,
                Status estado,
                const char *iataorigem,
                const char *iatadestino,
                const char *aircraft,
                const char *airline,
                const char *tracking_url);

const char* voo_get_id(const Voo *voo);
Status voo_get_status(const Voo *voo);
const char* voo_get_aircraft(const Voo *voo);
const char* voo_get_iata_origem(const Voo *voo);
const char* voo_get_iata_destino(const Voo *voo);
const Horario* voo_get_departure(const Voo *voo);
const Horario* voo_get_actual_departure(const Voo *voo);
const Horario* voo_get_arrival(const Voo *voo);
const Horario* voo_get_actual_arrival(const Voo *voo);
int voo_get_gate(const Voo *voo);
const char* voo_get_airline(const Voo *voo);
const char* voo_get_tracking_url(const Voo *voo);
int validar_flight_id(const char *s);
int voo_summary(const Voo *voo, char *buf, size_t bufsize);

#endif
