// Implementação do gestor global (estrutura opaca)
#include <stdlib.h>
#include "gestorglobal.h"
#include "aeroportos.h"
#include "voos.h"
#include "aeronaves.h"

// Definição interna da estrutura opaca declarada em gestorglobal.h
struct SistemaGestor {
    AeroportoPtrManager *aeroportos;
    VooPtrManager *voos;
    AeronavePtrManager *aeronaves;
    // futuros gestores: passageiros, reservas
};

void destruir_gestor_global(SistemaGestor *gestorGlobal) {
    if (!gestorGlobal) return;

    airport_ptr_manager_destroy(gestorGlobal->aeroportos); 
    voo_ptr_manager_destroy(gestorGlobal->voos);
    aeronave_ptr_manager_destroy(gestorGlobal->aeronaves);

    free(gestorGlobal);

}

SistemaGestor *criar_gestor_global(void) {
    SistemaGestor *gestorGlobal = malloc(sizeof(SistemaGestor));
    if(gestorGlobal==NULL) return NULL;

    gestorGlobal->aeroportos = airport_ptr_manager_create();
    gestorGlobal->voos = voo_ptr_manager_create();
    gestorGlobal->aeronaves = aeronave_ptr_manager_create();

    if(gestorGlobal->aeroportos == NULL || gestorGlobal->voos == NULL || gestorGlobal->aeronaves == NULL) {
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

