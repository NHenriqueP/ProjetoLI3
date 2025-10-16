#ifndef VOO
#define VOO

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

typedef struct horario {
    Date data;
    Hora hora;
} Horario;

typedef struct date {
    int year, month, day;
} Date;

typedef struct hora {
    int hora,minuto;
} Hora;

typedef enum {
    ON_TIME,
    DELAYED,
    CANCELLED
} Status;

#endif
