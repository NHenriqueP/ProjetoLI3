#ifndef VOO
#define VOO

#include <glib.h>
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

typedef struct voo Voo;
typedef struct voo_ptr_manager VooPtrManager;

// API
VooPtrManager* voo_ptr_manager_create();
void voo_ptr_manager_destroy(VooPtrManager *manager);
int voo_ptr_manager_add_from_ptr(VooPtrManager *manager, Voo *entidade);
Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id);
int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

guint voo_ptr_manager_count(const VooPtrManager *manager);
const Voo* voo_ptr_manager_get(const VooPtrManager *manager, guint index);

const char* voo_get_id(const Voo *voo);
Status voo_get_status(const Voo *voo);
const char* voo_get_aircraft(const Voo *voo);
char* voo_get_iata_origem(const Voo *voo);
char* voo_get_iata_destino(const Voo *voo);
const Horario* voo_get_departure(const Voo *voo);
const Horario* voo_get_actual_departure(const Voo *voo);
const Horario* voo_get_arrival(const Voo *voo);
const Horario* voo_get_actual_arrival(const Voo *voo);
int voo_get_gate(const Voo *voo);
const char* voo_get_airline(const Voo *voo);
const char* voo_get_tracking_url(const Voo *voo);
int validar_flight_id(const char *s);

// Summary formatting helper
int voo_summary(const Voo *voo, char *buf, size_t bufsize);

#endif
