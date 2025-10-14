#ifndef AERONAVE
#define AERONAVE

typedef struct aeronave {
    char id[10];
    char fabricante[50];
    char modelo[15];
    int anofabricacao;//data
    int capacidade;
    int alcance;
} Aeronave;



#endif
