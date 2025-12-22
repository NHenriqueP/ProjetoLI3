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

static const char *skip_spaces_const(const char *s) {
    if (!s) return NULL;
    while (*s && g_ascii_isspace(*s)) s++;
    return s;
}

static void free_airport_count(gpointer data) {
    AirportCount *ac = (AirportCount*)data;
    if (ac) {
        g_free((char*)ac->iata); 
        g_free(ac);              
    }
}

static int write_blank_line(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fputc('\n', f);
    fclose(f);
    return 1;
}

gboolean processar_voos_range(gpointer key, gpointer value, gpointer user_data) {
    Date *data_atual = (Date*)key;
    GPtrArray *lista_voos_dia = (GPtrArray*)value;
    void **args = (void**)user_data;

    Date *inicio = (Date*)args[0];
    Date *fim = (Date*)args[1];
    GHashTable *tabela_contagem = (GHashTable*)args[2];
    
    // Se data_atual > fim, paramos a procura imediatamente (Otimização da GTree)
    if (comparar_dates(data_atual, fim) > 0) return TRUE; // STOP

    // Se data_atual < inicio, continuamos a procurar mas não contamos nada
    if (comparar_dates(data_atual, inicio) < 0) return FALSE; // CONTINUE
    
    for (guint i = 0; i < lista_voos_dia->len; i++) {
        Voo *v = g_ptr_array_index(lista_voos_dia, i);
        const char *origem = voo_get_iata_origem(v); 

        AirportCount *ac = (AirportCount*)g_hash_table_lookup(tabela_contagem, origem);
        
        if (ac) {
            ac->count++;
            continue; // Próximo voo
        }
        else {
            ac = g_new0(AirportCount, 1);
            ac->iata = g_strdup(origem);
            ac->airport = NULL; // Será preenchido depois, se necessário
            ac->count = 1;
             g_hash_table_insert(tabela_contagem, 
                            (gpointer)origem, 
                            ac);
        }
    }

    return FALSE; // Continua para o próximo dia
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
    if (!aero_manager ) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        free(inicio);
        free(fim);
        return;
    }

    GHashTable *contagens = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_airport_count);
    if (!contagens) {
        if (write_blank_line(caminho_output)) (*Nout)++;
        free(inicio);
        free(fim);
        return;
    }

    void *args[3];
    args[0] = inicio;
    args[1] = fim;
    args[2] = contagens;

 // Passar o array 'args' como user_data
    VooPtrManager *voo_manager = gestor_global_get_voos(gestor);
    GTree *voos_por_data = manager_get_voos_por_data(voo_manager);
    if (voo_manager && voos_por_data) {
        g_tree_foreach(voos_por_data, processar_voos_range, args);
    }

    GHashTableIter iter;
    gpointer key, value;

    AirportCount *melhor = NULL;
    int max_voos = 0;
    g_hash_table_iter_init(&iter, contagens);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        
        AirportCount *ac = (AirportCount*)value;

        if (ac->count > max_voos) {
            max_voos = ac->count;
            melhor = ac;
        }
        else if (ac->count == max_voos) {
            if (melhor && strcmp(ac->iata, melhor->iata) < 0) {
                melhor = ac;
            }
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