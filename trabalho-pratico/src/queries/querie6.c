#include "querie6.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "gestorglobal.h"
#include "gestor_reserva.h"
#include "gestor_voo.h"
#include "gestor_passageiro.h"
#include "reservas.h"
#include "voos.h"
#include "passageiros.h"

// Cache: nationality -> (airport_iata -> count of landings)
typedef struct {
	GHashTable *nat_to_airport_counts; // key: char*, value: GHashTable* (key: char* airport, value: gint*)
	gboolean built;
} Q6Cache;

static Q6Cache *q6_cache = NULL;

static const char *skip_spaces_const(const char *s) {
	if (!s) return NULL;
	while (*s && g_ascii_isspace(*s)) s++;
	return s;
}

static void trim_trailing_ws(char *s) {
	if (!s) return;
	size_t len = strlen(s);
	while (len > 0 && g_ascii_isspace((unsigned char)s[len - 1])) {
		s[len - 1] = '\0';
		len--;
	}
}

static int write_blank_line(const char *path) {
	FILE *f = fopen(path, "w");
	if (!f) return 0;
	fputc('\n', f);
	fclose(f);
	return 1;
}

static void free_airport_counts(gpointer data) {
	GHashTable *h = (GHashTable*)data;
	if (h) g_hash_table_destroy(h);
}

static void q6_cache_free(Q6Cache *c) {
	if (!c) return;
	if (c->nat_to_airport_counts) g_hash_table_destroy(c->nat_to_airport_counts);
	g_free(c);
}

void querie6_cleanup_cache(void) {
	q6_cache_free(q6_cache);
	q6_cache = NULL;
}


static void build_cache_if_needed(SistemaGestor *gestor) {
	if (q6_cache && q6_cache->built) return;
	if (!q6_cache) {
		q6_cache = g_new0(Q6Cache, 1);
		if (!q6_cache) return;
		q6_cache->nat_to_airport_counts = g_hash_table_new_full(
			g_str_hash,
			g_str_equal,
			g_free,
			free_airport_counts
		);
		q6_cache->built = FALSE;
		if (!q6_cache->nat_to_airport_counts) {
			querie6_cleanup_cache();
			return;
		}
	}
	if (!gestor) return;

	ReservaPtrManager *reservas_m = gestor_global_get_reservas(gestor);
	VooPtrManager *voos_m = gestor_global_get_voos(gestor);
	PassageiroPtrManager *pass_m = gestor_global_get_passageiros(gestor);
	if (!reservas_m || !voos_m || !pass_m) {
		q6_cache->built = TRUE;
		return;
	}

	// Iterar todas as reservas
	guint total_reservas = reserva_ptr_manager_count(reservas_m);
	for (guint i = 0; i < total_reservas; i++) {
		const Reserva *r = reserva_ptr_manager_get(reservas_m, i);
		if (!r) continue;

		const char *doc = reserva_get_document_number(r);
		if (!doc || doc[0] == '\0') continue;

		const Passageiro *p = passageiro_ptr_manager_find_by_doc(pass_m, doc);
		if (!p) continue;

		const char *nat = passageiro_get_nationality(p);
		if (!nat || nat[0] == '\0') continue;

		int n_ids = reserva_get_n_flights(r);
		if (n_ids <= 0) continue;

		// Para CADA voo da reserva, contar o destino (cada aterragem conta)
		for (int j = 0; j < n_ids; j++) {
			const char *fid = reserva_get_flight_id_at(r, j);
			if (!fid) continue;
			const Voo *voo = voo_ptr_manager_find_by_id(voos_m, fid);
			if (!voo) continue;
			if (voo_get_status(voo) == CANCELLED) continue;

			const char *destino = voo_get_iata_destino(voo);
			if (!destino || destino[0] == '\0') continue;

			// Obter (ou criar) a hashtable de aeroportos para esta nacionalidade
			GHashTable *airport_counts = g_hash_table_lookup(q6_cache->nat_to_airport_counts, nat);
			if (!airport_counts) {
				char *nat_key = g_strdup(nat);
				if (!nat_key) continue;
				airport_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
				if (!airport_counts) {
					g_free(nat_key);
					continue;
				}
				g_hash_table_insert(q6_cache->nat_to_airport_counts, nat_key, airport_counts);
			}

			// Incrementar contador para este aeroporto
			gint *count = g_hash_table_lookup(airport_counts, destino);
			if (!count) {
				char *airport_key = g_strdup(destino);
				if (!airport_key) continue;
				count = g_new(gint, 1);
				if (!count) {
					g_free(airport_key);
					continue;
				}
				*count = 0;
				g_hash_table_insert(airport_counts, airport_key, count);
			}
			(*count)++;
		}
	}

	q6_cache->built = TRUE;
}

void execQ6(char linha[], SistemaGestor *gestor, int *Nout) {
	if (!linha || !gestor || !Nout) return;
	if (linha[0] != '6') return;

	char caminho_output[256];
	snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

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

	// Extrair nacionalidade (resto da linha, sem espaços à direita)
	char nationality[256];
	g_strlcpy(nationality, cursor ? cursor : "", sizeof(nationality));
	trim_trailing_ws(nationality);

	if (nationality[0] == '\0') {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}

	build_cache_if_needed(gestor);

	if (!q6_cache || !q6_cache->built || !q6_cache->nat_to_airport_counts) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}

	// Procurar a hashtable de aeroportos para esta nacionalidade
	GHashTable *airport_counts = g_hash_table_lookup(q6_cache->nat_to_airport_counts, nationality);
	if (!airport_counts || g_hash_table_size(airport_counts) == 0) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}

	// Encontrar o aeroporto com maior contagem (desempate: menor IATA lexicográfico)
	const char *best_airport = NULL;
	gint best_count = 0;

	GHashTableIter iter;
	gpointer key = NULL;
	gpointer value = NULL;
	g_hash_table_iter_init(&iter, airport_counts);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		const char *airport = (const char*)key;
		gint count = value ? *(gint*)value : 0;

		if (count > best_count || (count == best_count && (!best_airport || strcmp(airport, best_airport) < 0))) {
			best_airport = airport;
			best_count = count;
		}
	}

	FILE *out = fopen(caminho_output, "w");
	if (!out) return;

	if (!best_airport || best_count <= 0) {
		fputc('\n', out);
	} else {
		fprintf(out, "%s%c%d\n", best_airport, sep, best_count);
	}

	fclose(out);
	(*Nout)++;
}
