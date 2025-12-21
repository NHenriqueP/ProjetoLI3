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

typedef struct date_node {
    const Date *date_key;      // aponta para a Date dentro de um Voo (válida enquanto o gestor existir)
    GPtrArray *flights;        // array de (const Voo*) com actual departure nessa data
    struct date_node *left;
    struct date_node *right;
} DateNode;

static SistemaGestor *g_cached_gestor = NULL;
static DateNode *g_cached_root = NULL;

static void date_node_free(DateNode *n) {
    if (!n) return;
    date_node_free(n->left);
    date_node_free(n->right);
    if (n->flights) g_ptr_array_free(n->flights, TRUE);
    g_free(n);
}

static DateNode *date_bst_insert(DateNode *root, const Date *date_key, const Voo *voo) {
    if (!date_key || !voo) return root;
    if (!root) {
        DateNode *n = g_new0(DateNode, 1);
        if (!n) return NULL;
        n->date_key = date_key;
        n->flights = g_ptr_array_new();
        if (!n->flights) {
            g_free(n);
            return NULL;
        }
        g_ptr_array_add(n->flights, (gpointer)voo);
        return n;
    }

    int cmp = comparar_dates(date_key, root->date_key);
    if (cmp == 0) {
        g_ptr_array_add(root->flights, (gpointer)voo);
        return root;
    }
    if (cmp < 0) {
        root->left = date_bst_insert(root->left, date_key, voo);
    } else {
        root->right = date_bst_insert(root->right, date_key, voo);
    }
    return root;
}

static int ensure_date_index(SistemaGestor *gestor) {
    if (!gestor) return 0;
    if (g_cached_gestor == gestor && g_cached_root) return 1;

    if (g_cached_gestor != gestor) {
        date_node_free(g_cached_root);
        g_cached_root = NULL;
        g_cached_gestor = gestor;
    }

    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    if (!voos_manager) return 0;

    guint total_voos = voo_ptr_manager_count(voos_manager);
    for (guint i = 0; i < total_voos; i++) {
        const Voo *voo = voo_ptr_manager_get(voos_manager, i);
        if (!voo) continue;
        if (voo_get_status(voo) == CANCELLED) continue;

        const Horario *actual_departure = voo_get_actual_departure(voo);
        const Date *dep_date = horario_get_date(actual_departure);
        if (!dep_date) continue;

        DateNode *new_root = date_bst_insert(g_cached_root, dep_date, voo);
        if (!new_root && !g_cached_root) {
            // Falha na primeira alocação
            return 0;
        }
        g_cached_root = new_root;
    }
    return 1;
}

static int parse_date_ymd(const char *src, Date *out) {
    if (!src || !out) return 0;
    int y = 0, m = 0, d = 0;
    if (sscanf(src, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    set_date(out, y, m, d);
    return 1;
}

static const char *skip_spaces_const(const char *s) {
    if (!s) return NULL;
    while (*s && g_ascii_isspace(*s)) s++;
    return s;
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

static void accumulate_flights_for_date_node(
    const DateNode *node,
    GHashTable *contagens,
    AeroportoPtrManager *aero_manager,
    AirportCount **melhor
) {
    if (!node || !node->flights || !contagens || !aero_manager || !melhor) return;

    for (guint i = 0; i < node->flights->len; i++) {
        const Voo *voo = g_ptr_array_index(node->flights, i);
        if (!voo) continue;

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

        if (!*melhor || entry->count > (*melhor)->count ||
            (entry->count == (*melhor)->count && g_strcmp0(entry->iata, (*melhor)->iata) < 0)) {
            *melhor = entry;
        }
    }
}

static void bst_accumulate_range(
    const DateNode *root,
    const Date *inicio,
    const Date *fim,
    GHashTable *contagens,
    AeroportoPtrManager *aero_manager,
    AirportCount **melhor
) {
    if (!root || !inicio || !fim) return;
    int cmp_low = comparar_dates(root->date_key, inicio);
    int cmp_high = comparar_dates(root->date_key, fim);

    if (cmp_low < 0) {
        bst_accumulate_range(root->right, inicio, fim, contagens, aero_manager, melhor);
        return;
    }
    if (cmp_high > 0) {
        bst_accumulate_range(root->left, inicio, fim, contagens, aero_manager, melhor);
        return;
    }

    // root->date_key está no intervalo
    bst_accumulate_range(root->left, inicio, fim, contagens, aero_manager, melhor);
    accumulate_flights_for_date_node(root, contagens, aero_manager, melhor);
    bst_accumulate_range(root->right, inicio, fim, contagens, aero_manager, melhor);
}

void execQ3(char linha[], SistemaGestor *gestor, int *Nout) {
    if (!linha || !gestor || !Nout) return;
    if (linha[0] != '3') return;

    char caminho_output[256];
    snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

    // Novo modo: se existir um token 'S' depois do 3, usar '=' como separador no output.
    // Aceita: "3S <inicio> <fim>" e "3 S <inicio> <fim>".
    char sep = ';';
    const char *cursor = skip_spaces_const(linha + 1);
    if (!cursor || *cursor == '\0') {
        if (write_blank_line(caminho_output)) (*Nout)++;
        return;
    }
    if ((*cursor == 'S' || *cursor == 's') && (cursor[1] == '\0' || g_ascii_isspace(cursor[1]))) {
        sep = '=';
        cursor = skip_spaces_const(cursor + 1);
    }

    char start_raw[16] = {0};
    char end_raw[16] = {0};
    if (!cursor || sscanf(cursor, "%15s %15s", start_raw, end_raw) != 2) {
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

    AeroportoPtrManager *aero_manager = gestor_global_get_aeroportos(gestor);
    if (!aero_manager || !ensure_date_index(gestor)) {
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

    bst_accumulate_range(g_cached_root, inicio, fim, contagens, aero_manager, &melhor);

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
        int len = airport_summary_delim(melhor->airport, resumo, sizeof(resumo), sep);
        if (len <= 0) {
            fputc('\n', output);
            wrote = 1;
        } else {
            // Query 3 não inclui o tipo do aeroporto; remover último campo.
            char *last_sep = strrchr(resumo, sep);
            if (last_sep) {
                *last_sep = '\0';
                fprintf(output, "%s%c%d\n", resumo, sep, melhor->count);
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