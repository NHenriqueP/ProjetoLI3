#include "gestorglobal.h"
#include "aeroportos.h"

typedef struct 
{
    AeroportoPtrManager *aeroportos;
    // aeronaves;
    //passageiros;
    //reservas;
    //voos;
} GestorGlobal;

void destruir_gestor_global(SistemaGestor *gestorGlobal) {
    if (!gestorGlobal) return;

    airport_ptr_manager_destroy(gestorGlobal->aeroportos); 

    free(gestorGlobal);

}

SistemaGestor *criar_gestor_global() {
    SistemaGestor *gestorGlobal = malloc(sizeof(SistemaGestor));
    if(gestorGlobal==NULL) return NULL;

    gestorGlobal->aeroportos = airport_ptr_manager_create();

    if(gestorGlobal->aeroportos == NULL) {
        destruir_gestor_global(gestorGlobal);
        return NULL;
    }

    return gestorGlobal;
}

