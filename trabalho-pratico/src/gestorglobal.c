// Implementação do gestor global (estrutura opaca)
#include <stdlib.h>
#include "gestorglobal.h"
#include "aeroportos.h"
#include "voos.h"

// Definição interna da estrutura opaca declarada em gestorglobal.h
struct SistemaGestor {
    AeroportoPtrManager *aeroportos;
    VooPtrManager *voos;
    // futuros gestores: aeronaves, passageiros, reservas
};

void destruir_gestor_global(SistemaGestor *gestorGlobal) {
    if (!gestorGlobal) return;

    airport_ptr_manager_destroy(gestorGlobal->aeroportos); 
    voo_ptr_manager_destroy(gestorGlobal->voos);

    free(gestorGlobal);

}

SistemaGestor *criar_gestor_global(void) {
    SistemaGestor *gestorGlobal = malloc(sizeof(SistemaGestor));
    if(gestorGlobal==NULL) return NULL;

    gestorGlobal->aeroportos = airport_ptr_manager_create();
    gestorGlobal->voos = voo_ptr_manager_create();

    if(gestorGlobal->aeroportos == NULL || gestorGlobal->voos == NULL) {
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

