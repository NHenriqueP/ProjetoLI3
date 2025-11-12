// Query 2: Top N aeronaves com mais voos realizados (exclui CANCELLED)
// Opcional: filtro por fabricante
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <glib.h>

#include "querie2.h"
#include "aeronaves.h"
#include "voos.h"
#include "gestorglobal.h"

typedef struct {
    const Aeronave *aeronave;
    int count;
} AircraftCount;

gint compare_aircraft_counts(gconstpointer a, gconstpointer b) {
    const AircraftCount *lhs = *((AircraftCount **)a);
    const AircraftCount *rhs = *((AircraftCount **)b);
    if (!lhs || !rhs) return 0;
    if (lhs->count < rhs->count) return 1;
    if (lhs->count > rhs->count) return -1;
    const char *id_lhs = aeronave_get_id(lhs->aeronave);
    const char *id_rhs = aeronave_get_id(rhs->aeronave);
    if (!id_lhs) id_lhs = "";
    if (!id_rhs) id_rhs = "";
    return strcmp(id_lhs, id_rhs);
}

static int escrever_resultados(const GPtrArray *ordenado, int limite, const char *caminho_output) {
    FILE *output = fopen(caminho_output, "w");
    if (!output) return 0;
    if (!ordenado || limite <= 0 || ordenado->len == 0) {
        fputc('\n', output);
        fclose(output);
        return 1;
    }

    int max = limite;
    if (max > (int)ordenado->len) max = (int)ordenado->len;
    for (int i = 0; i < max; i++) {
        const AircraftCount *entry = g_ptr_array_index((GPtrArray*)ordenado, i);
        if (!entry || !entry->aeronave) continue;
        const char *id = aeronave_get_id(entry->aeronave);
        const char *fabricante = aeronave_get_fabricante(entry->aeronave);
        const char *modelo = aeronave_get_modelo(entry->aeronave);
        if (!id) id = "";
        if (!fabricante) fabricante = "";
        if (!modelo) modelo = "";
        fprintf(output, "%s,%s,%s,%d\n", id, fabricante, modelo, entry->count);
    }
    fclose(output);
    return 1;
}

static char *trim_leading_ws(char *s) {
    if (!s) return NULL;
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

static void trim_trailing_ws(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

void execQ2(char linha[], SistemaGestor *gestor, int *Nout) {
    if (!linha || !gestor || !Nout) return;
    if (linha[0] != '2') return;

    char *cursor = trim_leading_ws(linha + 1);
    if (!cursor || *cursor == '\0') return;

    char *endptr = NULL;
    long parsed = strtol(cursor, &endptr, 10);
    if (endptr == cursor || parsed <= 0) parsed = 0;
    if (parsed > INT_MAX) parsed = INT_MAX;

    cursor = trim_leading_ws(endptr);
    char *manufacturer = NULL;
    if (cursor && *cursor) {
        manufacturer = cursor;
        trim_trailing_ws(manufacturer);
        if (*manufacturer == '\0') manufacturer = NULL;
    }

    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    AeronavePtrManager *aeronaves_manager = gestor_global_get_aeronaves(gestor);

    char caminho_output[256];
    snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

    if (!voos_manager || !aeronaves_manager || aeronave_ptr_manager_count(aeronaves_manager) == 0) {
        if (escrever_resultados(NULL, 0, caminho_output)) (*Nout)++;
        return;
    }

    GHashTable *contagens = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);

    guint total_aeronaves = aeronave_ptr_manager_count(aeronaves_manager);
    for (guint i = 0; i < total_aeronaves; i++) {
        const Aeronave *a = aeronave_ptr_manager_get(aeronaves_manager, i);
        if (!a) continue;
        const char *id = aeronave_get_id(a);
        if (!id || *id == '\0') continue;
        if (manufacturer) {
            const char *fab = aeronave_get_fabricante(a);
            if (!fab || g_ascii_strcasecmp(fab, manufacturer) != 0) continue;
        }
        AircraftCount *entry = g_new0(AircraftCount, 1);
        entry->aeronave = a;
        g_hash_table_insert(contagens, (gpointer)id, entry);
    }

    guint total_voos = voo_ptr_manager_count(voos_manager);
    for (guint i = 0; i < total_voos; i++) {
        const Voo *voo = voo_ptr_manager_get(voos_manager, i);
        if (!voo) continue;
        if (voo_get_status(voo) == CANCELLED) continue;
        const char *aircraft_id = voo_get_aircraft(voo);
        if (!aircraft_id || aircraft_id[0] == '\0') continue;

        AircraftCount *entry = g_hash_table_lookup(contagens, aircraft_id);
        if (!entry) continue;
        entry->count++;
    }

    GPtrArray *ordenado = g_ptr_array_new();
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, contagens);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        AircraftCount *entry = value;
        if (entry && entry->count > 0) g_ptr_array_add(ordenado, entry);
    }

    g_ptr_array_sort(ordenado, compare_aircraft_counts);

    int wrote = escrever_resultados(ordenado, (int)parsed, caminho_output);

    g_ptr_array_free(ordenado, TRUE);
    g_hash_table_destroy(contagens);

    if (wrote) (*Nout)++;
}