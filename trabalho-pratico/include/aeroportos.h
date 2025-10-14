#ifndef AEROPORTO
#define AEROPORTO


typedef struct aeroporto {
    char iata[4];
    char nome[50];
    char cidade[20];
    char pais[20];
    char latitude[13];
    char longitude[13];
    char icao[5];
    char tipo[10];//mudar
} Aeroporto;

#endif 
