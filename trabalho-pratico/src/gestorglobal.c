// Implementação do gestor global (estrutura opaca)
#include <stdlib.h>
#include "gestorglobal.h"
#include "gestor_aeroporto.h"
#include "gestor_voo.h"
#include "gestor_aeronave.h"
#include "gestor_passageiro.h"
#include "gestor_reserva.h"

// Definição interna da estrutura opaca declarada em gestorglobal.h
struct SistemaGestor {
    AeroportoPtrManager *aeroportos;
    VooPtrManager *voos;
    AeronavePtrManager *aeronaves;
    PassageiroPtrManager *passageiros;
    ReservaPtrManager *reservas;
};

void destruir_gestor_global(SistemaGestor *gestorGlobal) {
    if (!gestorGlobal) return;

    airport_ptr_manager_destroy(gestorGlobal->aeroportos); 
    voo_ptr_manager_destroy(gestorGlobal->voos);
    aeronave_ptr_manager_destroy(gestorGlobal->aeronaves);
    passageiro_ptr_manager_destroy(gestorGlobal->passageiros);
    reserva_ptr_manager_destroy(gestorGlobal->reservas);

    free(gestorGlobal);

}

SistemaGestor *criar_gestor_global(void) {
    SistemaGestor *gestorGlobal = malloc(sizeof(SistemaGestor));
    if(gestorGlobal==NULL) return NULL;

    gestorGlobal->aeroportos = airport_ptr_manager_create();
    gestorGlobal->voos = voo_ptr_manager_create();
    gestorGlobal->aeronaves = aeronave_ptr_manager_create();
    gestorGlobal->passageiros = passageiro_ptr_manager_create();
    gestorGlobal->reservas = reserva_ptr_manager_create();

    if(gestorGlobal->aeroportos == NULL || gestorGlobal->voos == NULL || gestorGlobal->aeronaves == NULL ||
       gestorGlobal->passageiros == NULL || gestorGlobal->reservas == NULL) {
        destruir_gestor_global(gestorGlobal);
        return NULL;
    }

    return gestorGlobal;
}

AeroportoPtrManager *gestor_global_get_aeroportos(SistemaGestor *gestor) {
    if (!gestor) return NULL;
    return gestor->aeroportos;
}

VooPtrManager *gestor_global_get_voos(SistemaGestor *gestor) {
    if (!gestor) return NULL;
    return gestor->voos;
}

AeronavePtrManager *gestor_global_get_aeronaves(SistemaGestor *gestor) {
    if (!gestor) return NULL;
    return gestor->aeronaves;
}

PassageiroPtrManager *gestor_global_get_passageiros(SistemaGestor *gestor) {
    if (!gestor) return NULL;
    return gestor->passageiros;
}

ReservaPtrManager *gestor_global_get_reservas(SistemaGestor *gestor) {
    if (!gestor) return NULL;
    return gestor->reservas;
}

