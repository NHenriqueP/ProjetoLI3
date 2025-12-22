#ifndef VOO
#define VOO

#include <stddef.h>
#include <glib.h>

typedef struct Date Date;

typedef struct Hora  Hora;

typedef struct Horario Horario;

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

void horario_free(Horario *h);
void horario_copy(Horario *dest, const Horario *src);
void horario_reset(Horario *h);
Date *date_new(void);
const Date *horario_get_date(const Horario *h);
Horario *horario_new(void);
void set_date(Date *d, int year, int month, int day);
void passar_date (Date *dest, const Date *src);
gint comparar_dates_dia(gconstpointer a, gconstpointer b);
void set_horario(Horario *h, int year, int month, int day, int hora, int minuto);
int comparar_horarios(const Horario *a, const Horario *b);
int comparar_dates(const Date *a, const Date *b);

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