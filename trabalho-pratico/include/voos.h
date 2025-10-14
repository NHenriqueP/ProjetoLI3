#ifndef VOO
#define VOO

typedef struct voo {
    char idvoo[8];
    /*
    Date departure;
    Date actualdeparture;
    Date arrival;
    Date actualarrival;
    */
    int gate;
    Status estado;
    char iataorigem[4];
    char iatadestino[4];
    char aircraft[10];
    char airline[50];
    char *tracking_url;
} Voo;


typedef struct date {
    int year, month, day, hour, minute;
} Date;


typedef enum {
    ON_TIME,
    DELAYED,
    CANCELLED
} Status;

#endif
