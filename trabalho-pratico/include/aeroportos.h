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
    char *tipo;
} Aeroporto;

typedef struct {
    Aeroporto *airports;
    int count;
    int capacity;
} AeroportoManager;






#endif 
