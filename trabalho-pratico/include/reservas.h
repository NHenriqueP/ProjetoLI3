#ifndef RESERVAS
#define RESERVAS
#include <stdbool.h>
typedef struct reserva {
    char id[11];
    char vooid[9];
    char nif[10];
    char banco[4];
    char preco[10];
    bool bagagemextra;
    bool prioritario;
    //definir qr code;
} Reserva;



#endif