// Query 2: Top N aeronaves com mais voos realizados (exclui CANCELLED)
// Opcional: filtro por fabricante
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "querie2.h"
#include "aeronaves.h"
#include "voos.h"
#include "gestorglobal.h"

typedef struct {
    char id[16];
    char fabricante[32];
    char modelo[32];
    int total_voos;
} Q2Item;

// qsort comparator over an array of pointers (arr->pdata): a and b are Q2Item* pointers' addresses
static int cmp_q2(const void *a, const void *b) {
    const Q2Item *x = *(const Q2Item* const*)a;
    const Q2Item *y = *(const Q2Item* const*)b;
    if (x->total_voos != y->total_voos) return (y->total_voos - x->total_voos);
    return strcmp(x->id, y->id);
}

void execQ2(char linha[], SistemaGestor *gestor, int *Nout) {
    if (!linha || !gestor || !Nout) return;
    if (linha[0] != '2') return;

    // Parse: "2 <N> [<fabricante>]"
    char *p = &linha[1];
    while (*p == ' ') p++;
    if (!*p) return;
    char *endptr = NULL;
    long topN = strtol(p, &endptr, 10);
    if (p == endptr || topN <= 0) return;
    p = endptr;
    while (*p == ' ') p++;
    char fabricante_filter[64];
    fabricante_filter[0] = '\0';
    if (*p) {
        // Lê até ao fim da palavra
        size_t len = 0;
        while (p[len] && p[len] != '\n' && p[len] != '\r' && p[len] != ' ' && len + 1 < sizeof(fabricante_filter)) len++;
        memcpy(fabricante_filter, p, len);
        fabricante_filter[len] = '\0';
    }

    AeronavePtrManager *am = gestor_global_get_aeronaves(gestor);
    VooPtrManager *vm = gestor_global_get_voos(gestor);
    if (!am || !vm) return;

    // Construir mapa id->info
    GHashTable *map = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    for (guint i = 0; i < am->aeronaves->len; i++) {
        Aeronave *a = g_array_index(am->aeronaves, Aeronave*, i);
        if (!a) continue;
        const char *fab = aeronave_get_fabricante(a);
        if (fabricante_filter[0] != '\0' && (!fab || strcmp(fab, fabricante_filter) != 0)) continue;
        const char *aid = aeronave_get_id(a);
        if (!aid) continue;
        Q2Item *item = g_new0(Q2Item, 1);
        g_strlcpy(item->id, aid, sizeof(item->id));
        g_strlcpy(item->fabricante, fab ? fab : "", sizeof(item->fabricante));
        g_strlcpy(item->modelo, aeronave_get_modelo(a) ? aeronave_get_modelo(a) : "", sizeof(item->modelo));
        item->total_voos = 0;
        g_hash_table_insert(map, g_strdup(item->id), item);
    }

    // Contabilizar voos (exclui CANCELLED)
    for (guint j = 0; j < vm->flights->len; j++) {
        Voo *v = g_array_index(vm->flights, Voo*, j);
        if (!v) continue;
        if (v->estado == CANCELLED) continue;
        Q2Item *item = (Q2Item*)g_hash_table_lookup(map, v->aircraft);
        if (item) item->total_voos++;
    }

    // Extrair, ordenar e emitir top N
    GPtrArray *arr = g_ptr_array_new();
    GHashTableIter it; gpointer key, value;
    g_hash_table_iter_init(&it, map);
    while (g_hash_table_iter_next(&it, &key, &value)) {
        g_ptr_array_add(arr, value);
    }

    qsort(arr->pdata, arr->len, sizeof(gpointer), cmp_q2);

    char caminho_output[256];
    snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);
    FILE *out = fopen(caminho_output, "w");
    if (!out) { g_ptr_array_free(arr, TRUE); g_hash_table_destroy(map); return; }

    guint emit = (guint)topN;
    if (emit > arr->len) emit = arr->len;
    for (guint i = 0; i < emit; i++) {
        Q2Item *it2 = (Q2Item*)arr->pdata[i];
        fprintf(out, "%s,%s,%s,%d\n", it2->id, it2->fabricante, it2->modelo, it2->total_voos);
    }
    fclose(out);
    (*Nout)++;

    // Limpeza (keys são freed automaticamente pelo free function da hash)
    g_ptr_array_free(arr, TRUE);
    g_hash_table_destroy(map);
}