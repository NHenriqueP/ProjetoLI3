#ifndef GESTORGLOBAL_H
#define GESTORGLOBAL_H

#include "aeroportos.h" // para o tipo AeroportoPtrManager
#include "voos.h"       // para o tipo VooPtrManager
#include "aeronaves.h"  // para o tipo AeronavePtrManager

// Opaque forward declaration to hide internals from users of this API
typedef struct SistemaGestor SistemaGestor;

void destruir_gestor_global(SistemaGestor *gestorGlobal);
SistemaGestor *criar_gestor_global(void);

// Acesso controlado aos subgestores
AeroportoPtrManager *gestor_global_get_aeroportos(SistemaGestor *gestor);
VooPtrManager *gestor_global_get_voos(SistemaGestor *gestor);
AeronavePtrManager *gestor_global_get_aeronaves(SistemaGestor *gestor);

#endif // GESTORGLOBAL_H