#ifndef AEROPORTO
#define AEROPORTO


typedef struct aeroporto {
    char iata[4];
    char nome[100];
    char cidade[70];
    char pais[3];
    char latitude[20];
    char longitude[20];
    char icao[5];
    Tipo tipo;
} Aeroporto;


typedef enum {
    small_airport,
    medium_airport,
    large_airport,
    heliport,
    seaplane_base,
} Tipo;


#endif 
