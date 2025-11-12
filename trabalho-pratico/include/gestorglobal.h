#ifndef GESTORGLOBAL_H
#define GESTORGLOBAL_H

#include "gestor_aeroporto.h"
#include "gestor_voo.h"
#include "gestor_aeronave.h"
#include "gestor_passageiro.h"
#include "gestor_reserva.h"

// Opaque forward declaration to hide internals from users of this API
typedef struct SistemaGestor SistemaGestor;

void destruir_gestor_global(SistemaGestor *gestorGlobal);
SistemaGestor *criar_gestor_global(void);

// Acesso controlado aos subgestores
AeroportoPtrManager *gestor_global_get_aeroportos(SistemaGestor *gestor);
VooPtrManager *gestor_global_get_voos(SistemaGestor *gestor);
AeronavePtrManager *gestor_global_get_aeronaves(SistemaGestor *gestor);
PassageiroPtrManager *gestor_global_get_passageiros(SistemaGestor *gestor);
ReservaPtrManager *gestor_global_get_reservas(SistemaGestor *gestor);

#endif // GESTORGLOBAL_H