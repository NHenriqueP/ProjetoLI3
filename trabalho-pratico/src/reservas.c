#include "reservas.h"
#include <stdlib.h>

typedef struct reserva {
    char *id;
    char *vooid;
    char *nif;
    char *banco;
    char *preco;
    char *bagagemextra;
    char *prioritario;
    char *qrcode;
} Reserva;