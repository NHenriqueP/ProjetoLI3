#include "aeronaves.h"
#include "passageiros.h"
#include "reservas.h"
#include "voos.h"
#include "aeroportos.h"

typedef struct SistemaGestor SistemaGestor;

void destruir_gestor_global(SistemaGestor *gestorGlobal);
SistemaGestor *criar_gestor_global();