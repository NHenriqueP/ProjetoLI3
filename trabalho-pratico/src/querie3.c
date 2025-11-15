// Query 3: aeroporto com mais partidas num intervalo de datas (exclui CANCELLED)
#include "querie3.h"

#include "aeroportos.h"
#include "gestor_aeroporto.h"
#include "voos.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    Aeroporto *airport;
    const char *iata;
    int count;
} AirportCount;

static int parse_date_ymd(const char *src, Date *out) {
    if (!src || !out) return 0;
    int y = 0, m = 0, d = 0;
    if (sscanf(src, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    set_date(out, y, m, d);
    return 1;
}


static int date_in_range(const Date *target, const Date *start, const Date *end) {
    return comparar_dates(target, start) >= 0 && comparar_dates(target, end) <= 0;
}

static void free_airport_count(gpointer data) {
    g_free(data);
}

static int write_blank_line(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fputc('\n', f);
    fclose(f);
    return 1;
}

void execQ3(char linha[], SistemaGestor *gestor, int *Nout) {
    if (!linha || !gestor || !Nout) return;
    if (linha[0] != '3') return;

    char caminho_output[256];
    snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

    char start_raw[16] = {0};
    char end_raw[16] = {0};
    if (sscanf(linha + 1, " %15s %15s", start_raw, end_raw) != 2) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        return;
    }

    Date *inicio = date_new();
    Date *fim = date_new();
    if (!parse_date_ymd(start_raw, inicio) || !parse_date_ymd(end_raw, fim) || comparar_dates(inicio, fim) > 0) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        free(inicio);
        free(fim);
        return;
    }

    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    AeroportoPtrManager *aero_manager = gestor_global_get_aeroportos(gestor);
    if (!voos_manager || !aero_manager) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        free(inicio);
        free(fim);
        return;
    }

    GHashTable *contagens = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free_airport_count);
    if (!contagens) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        free(inicio);
        free(fim);
        return;
    }

    AirportCount *melhor = NULL;

    guint total_voos = voo_ptr_manager_count(voos_manager);
    for (guint i = 0; i < total_voos; i++) {
        const Voo *voo = voo_ptr_manager_get(voos_manager, i);
        if (!voo) continue;
        if (voo_get_status(voo) == CANCELLED) continue;

        const Horario *actual_departure = voo_get_actual_departure(voo);
        const Date *dep_date = horario_get_date(actual_departure);
        if (!actual_departure) continue;
        if (!date_in_range(dep_date, inicio, fim)) continue;

        const char *origem = voo_get_iata_origem(voo);
        if (!origem || origem[0] == '\0') continue;

        AirportCount *entry = g_hash_table_lookup(contagens, origem);
        if (!entry) {
            Aeroporto *airport = airport_ptr_manager_find_by_iata(aero_manager, origem);
            if (!airport) continue;
            char *key = g_strdup(origem);
            if (!key) continue;
            entry = g_new0(AirportCount, 1);
            if (!entry) {
                g_free(key);
                continue;
            }
            entry->airport = airport;
            entry->iata = key;
            g_hash_table_insert(contagens, key, entry);
        }

        entry->count++;

        if (!melhor || entry->count > melhor->count ||
            (entry->count == melhor->count && g_strcmp0(entry->iata, melhor->iata) < 0)) {
            melhor = entry;
        }
    }

    FILE *output = fopen(caminho_output, "w");
    if (!output) {
        g_hash_table_destroy(contagens);
        free(inicio);
        free(fim);
        return;
    }

    int wrote = 0;

    if (!melhor || melhor->count == 0 || !melhor->airport) {
        fputc('\n', output);
        wrote = 1;
    } else {
        char resumo[512];
        int len = airport_summary(melhor->airport, resumo, sizeof(resumo));
        if (len <= 0) {
            fputc('\n', output);
            wrote = 1;
        } else {
            char *last_comma = strrchr(resumo, ',');
            if (last_comma) {
                *last_comma = '\0';
                fprintf(output, "%s,%d\n", resumo, melhor->count);
                wrote = 1;
            } else {
                fputc('\n', output);
                wrote = 1;
            }
        }
    }
    free(inicio);
    free(fim);
    fclose(output);
    g_hash_table_destroy(contagens);

    if (wrote) (*Nout)++;
}
