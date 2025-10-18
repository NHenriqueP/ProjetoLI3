#ifndef AEROPORTO
#define AEROPORTO


typedef struct aeroporto {
    char *iata;
    char *nome;
    char *cidade;
    char *pais;
    char *latitude;
    char *longitude;
    char *icao;
    Tipo tipo;
} Aeroporto;

typedef struct {
    Aeroporto *airports;
    int count;
    int capacity;
} AeroportoManager;


typedef enum { //eliminar se não necessario
    small_airport,
    medium_airport,
    large_airport,
    heliport,
    seaplane_base,
} Tipo;



#endif 
