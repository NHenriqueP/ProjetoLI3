#include "aeronaves.h"

#include <glib.h>
#include <stdlib.h>

struct Aeronave {
    char *id;
    char *fabricante;
    char *modelo;
};

Aeronave* aeronave_create(const char *id,
                          const char *fabricante,
                          const char *modelo) {
    if (!id || !fabricante || !modelo) return NULL;

    Aeronave *a = g_new0(Aeronave, 1);
    if (!a) return NULL;

    a->id = g_strdup(id);
    a->fabricante = g_strdup(fabricante);
    a->modelo = g_strdup(modelo);

    if (!a->id || !a->fabricante || !a->modelo) {
        aeronave_destroy(a);
        return NULL;
    }

    return a;
}

void aeronave_destroy(Aeronave *a) {
    if (!a) return;

    g_free(a->id);
    g_free(a->fabricante);
    g_free(a->modelo);
    g_free(a);
}


const char* aeronave_get_id(const Aeronave *a) {
    if (a) return a->id;
    return NULL;
}

const char* aeronave_get_fabricante(const Aeronave *a) {
    if (a) return a->fabricante;
    return NULL;
}

const char* aeronave_get_modelo(const Aeronave *a) {
    if (a) return a->modelo;
    return NULL;
}

