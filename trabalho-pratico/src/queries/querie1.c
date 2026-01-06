// Query 1: Resumo do aeroporto dado o código IATA
#include "querie1.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "aeroportos.h"
#include "gestor_aeroporto.h"
#include "gestor_reserva.h"
#include "gestor_voo.h"
#include "gestorglobal.h"
#include "reservas.h"
#include "voos.h"

static char *skip_spaces(char *s) {
    if (!s) return NULL;
    while (*s && g_ascii_isspace(*s)) s++;
    return s;
}

// Extrai até max_ids flight ids de uma string no formato "['AB12345', 'CD67890']"
// Nota: fase 2 tem flight ids com 7 ou 8 caracteres.
static int extract_flight_ids_fast(const char *src, char ids[][9], int max_ids) {
    if (!src || max_ids <= 0) return 0;
    int count = 0;
    const char *p = src;
    while (*p && count < max_ids) {
        while (*p && *p != '\'' && *p != '"') p++;
        if (!*p) break;
        char quote = *p++;
        int i = 0;
        while (*p && *p != quote && i < 8) {
            ids[count][i++] = *p++;
        }
        ids[count][i] = '\0';
        // avançar até fechar aspas
        while (*p && *p != quote) p++;
        if (*p == quote) p++;
        if (i == 7 || i == 8) count++;
    }
    return count;
}

typedef struct {
    long aterragem;
    long partida;
} Q1Counts;

typedef struct {
    GHashTable *iata_to_counts; // key: char* (IATA), value: Q1Counts*
    gboolean built;
} Q1Cache;

static Q1Cache *q1_cache = NULL;

static void q1_cache_free(Q1Cache *c) {
    if (!c) return;
    if (c->iata_to_counts) g_hash_table_destroy(c->iata_to_counts);
    g_free(c);
}

void querie1_cleanup_cache(void) {
    q1_cache_free(q1_cache);
    q1_cache = NULL;
}

static Q1Counts *q1_get_or_create_counts(const char *iata) {
    if (!q1_cache || !q1_cache->iata_to_counts || !iata || iata[0] == '\0') return NULL;
    Q1Counts *c = g_hash_table_lookup(q1_cache->iata_to_counts, iata);
    if (c) return c;
    char *k = g_strdup(iata);
    if (!k) return NULL;
    c = g_new0(Q1Counts, 1);
    if (!c) {
        g_free(k);
        return NULL;
    }
    g_hash_table_insert(q1_cache->iata_to_counts, k, c);
    return c;
}

static void q1_build_cache_if_needed(SistemaGestor *gestor) {
    if (q1_cache && q1_cache->built) return;
    if (!q1_cache) {
        q1_cache = g_new0(Q1Cache, 1);
        if (!q1_cache) return;
        q1_cache->iata_to_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
        q1_cache->built = FALSE;
        if (!q1_cache->iata_to_counts) {
            querie1_cleanup_cache();
            return;
        }
    }
    if (!gestor) {
        q1_cache->built = TRUE;
        return;
    }

    ReservaPtrManager *reservas_manager = gestor_global_get_reservas(gestor);
    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    if (!reservas_manager || !voos_manager) {
        q1_cache->built = TRUE;
        return;
    }

    guint total_reservas = reserva_ptr_manager_count(reservas_manager);
    for (guint i = 0; i < total_reservas; i++) {
        const Reserva *reserva = reserva_ptr_manager_get(reservas_manager, i);
        if (!reserva) continue;
        const char *flight_ids_raw = reserva_get_flight_ids(reserva);
        if (!flight_ids_raw || flight_ids_raw[0] == '\0') continue;

        char ids[2][9] = {{0}};
        int n_ids = extract_flight_ids_fast(flight_ids_raw, ids, 2);
        if (n_ids <= 0) continue;

        for (int j = 0; j < n_ids; j++) {
            const Voo *voo = voo_ptr_manager_find_by_id(voos_manager, ids[j]);
            if (!voo) continue;
            if (voo_get_status(voo) == CANCELLED) continue;
            const char *origem = voo_get_iata_origem(voo);
            const char *destino = voo_get_iata_destino(voo);
            if (origem && origem[0] != '\0') {
                Q1Counts *c = q1_get_or_create_counts(origem);
                if (c) c->partida++;
            }
            if (destino && destino[0] != '\0') {
                Q1Counts *c = q1_get_or_create_counts(destino);
                if (c) c->aterragem++;
            }
        }
    }

    q1_cache->built = TRUE;
}

static void q1_lookup_counts(SistemaGestor *gestor, const char *codigo_iata, long *aterragens, long *partidas) {
	if (!aterragens || !partidas) return;
	*aterragens = 0;
	*partidas = 0;
	if (!codigo_iata || codigo_iata[0] == '\0') return;
	q1_build_cache_if_needed(gestor);
	if (!q1_cache || !q1_cache->iata_to_counts) return;
	Q1Counts *c = g_hash_table_lookup(q1_cache->iata_to_counts, codigo_iata);
	if (!c) return;
	*aterragens = c->aterragem;
	*partidas = c->partida;
}

void execQ1(char linha[],SistemaGestor *gestor, int *Nout) {
    char codigo_iata[5] = {0};
    char sep = ';';

    if (linha[0] != '1' || linha[1] == '\0') {
        // Não é a Query 1 ou a linha está muito curta
        return; 
    }

    char *p = skip_spaces(&linha[1]);
    if (!p || *p == '\0') return;

    // Novo modo: se existir um token 'S' depois do 1, usar '=' como separador.
    // Aceita: "1S <IATA>" e "1 S <IATA>".
    // Não deve confundir com IATA a começar por 'S' (ex.: "1 SFO").
    if ((*p == 'S' || *p == 's') && (p[1] == '\0' || g_ascii_isspace(p[1]))) {
        sep = '=';
        p = skip_spaces(p + 1);
        if (!p || *p == '\0') return;
    }

    // Ler token do IATA (3 ou 4 chars, por compatibilidade)
    char token[8] = {0};
    size_t len = 0;
    while (p[len] && !g_ascii_isspace(p[len]) && len < sizeof(token) - 1) len++;
    if (len != 3 && len != 4) return;
    memcpy(token, p, len);
    token[len] = '\0';
    for (size_t i = 0; i < len; i++) token[i] = (char)g_ascii_toupper((gchar)token[i]);
    g_strlcpy(codigo_iata, token, sizeof(codigo_iata));

    char caminho_output[256];
    snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

    FILE *output_file = fopen(caminho_output, "w");
    if (!output_file) return;

    Aeroporto *aeroporto_ptr = airport_ptr_manager_find_by_iata(
        gestor_global_get_aeroportos(gestor),
        codigo_iata
    );

    if (aeroporto_ptr != NULL) {
        char buf[512];
        long aterragem = 0;
        long partida = 0;
        q1_lookup_counts(gestor, codigo_iata, &aterragem, &partida);

        if (airport_summary_delim(aeroporto_ptr, buf, sizeof(buf), sep) > 0) {
            fprintf(output_file, "%s%c%ld%c%ld\n", buf, sep, aterragem, sep, partida);
        } else {
            fprintf(output_file, "\n");
        }
    } else {
        fprintf(output_file, "\n");
    }

    fclose(output_file);
    (*Nout)++;
}