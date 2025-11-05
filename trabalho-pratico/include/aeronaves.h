#ifndef AERONAVE
#define AERONAVE

typedef struct 
{
    AeroportoPtrManager *aeroportos;
    // aeronaves;
    //passageiros;
    //reservas;
    //voos;
} SistemaGestor;

typedef struct aeronave {
    char id[9];
    char fabricante[15];
    char modelo[15];
    int anofabricacao;
    int capacidade;
    int alcance;
} Aeronave;



#endif
