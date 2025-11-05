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

typedef struct voo {
    char idvoo[8];
    Horario departure;
    Horario actualdeparture;
    Horario arrival;
    Horario actualarrival;
    int gate;
    Status estado;
    char iataorigem[4];
    char iatadestino[4];
    char aircraft[10];
    char airline[50];
    char *tracking_url;
} Voo;

// Pointer-based flights manager (stores Voo* created by parser)
typedef struct {
    GArray *flights; /* array of (Voo*) */
} VooPtrManager;

// API
VooPtrManager* voo_ptr_manager_create();
void voo_ptr_manager_destroy(VooPtrManager *manager);
int voo_ptr_manager_add_from_ptr(VooPtrManager *manager, Voo *entidade);
Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id);
int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Summary formatting helper
int voo_summary(const Voo *voo, char *buf, size_t bufsize);

#endif
