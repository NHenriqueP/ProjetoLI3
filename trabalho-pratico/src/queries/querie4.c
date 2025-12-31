#include "querie4.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "gestorglobal.h"
#include "gestor_reserva.h"
#include "gestor_voo.h"
#include "gestor_passageiro.h"
#include "reservas.h"
#include "voos.h"
#include "passageiros.h"

typedef struct {
	GHashTable *week_to_top10; // key: gint64* (week_start_day), value: GPtrArray* of (Passageiro*)
	GArray *sorted_week_starts; // elements: gint64
	gboolean built;
} Q4Cache;

static Q4Cache *q4_cache = NULL;

static const char *skip_spaces_const(const char *s) {
	if (!s) return NULL;
	while (*s && g_ascii_isspace(*s)) s++;
	return s;
}

static int write_blank_line(const char *path) {
	FILE *f = fopen(path, "w");
	if (!f) return 0;
	fputc('\n', f);
	fclose(f);
	return 1;
}

static void free_top10_array(gpointer data) {
	GPtrArray *a = (GPtrArray*)data;
	if (a) g_ptr_array_free(a, TRUE);
}

static void q4_cache_free(Q4Cache *c) {
	if (!c) return;
	if (c->week_to_top10) g_hash_table_destroy(c->week_to_top10);
	if (c->sorted_week_starts) g_array_free(c->sorted_week_starts, TRUE);
	g_free(c);
}

void querie4_cleanup_cache(void) {
	q4_cache_free(q4_cache);
	q4_cache = NULL;
}

static long long days_from_civil(int y, unsigned m, unsigned d) {
	y -= (m <= 2);
	const int era = (y >= 0 ? y : y - 399) / 400;
	const unsigned yoe = (unsigned)(y - era * 400);
	const unsigned doy = (153 * (m + (m > 2 ? (unsigned)-3 : 9)) + 2) / 5 + d - 1;
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	return (long long)(era * 146097 + (int)doe - 719468);
}

static int parse_date_ymd(const char *src, gint64 *out_day) {
	if (!src || !out_day) return 0;
	int y = 0, m = 0, d = 0;
	if (sscanf(src, "%d-%d-%d", &y, &m, &d) != 3) return 0;
	if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
	*out_day = (gint64)days_from_civil(y, (unsigned)m, (unsigned)d);
	return 1;
}

static gint64 weekday_sun0(gint64 day_number) {
	// day_number: days since 1970-01-01, where 1970-01-01 is Thursday.
	// Return 0=Sunday..6=Saturday.
	gint64 w = (day_number + 4) % 7;
	if (w < 0) w += 7;
	return w;
}

static gint64 week_start_sunday(gint64 day_number) {
	return day_number - weekday_sun0(day_number);
}

typedef struct {
	Passageiro *p;
	double total;
} TopEntry;

static int doc_compare(const char *a, const char *b) {
	if (!a) a = "";
	if (!b) b = "";
	return strcmp(a, b);
}

static gboolean entry_better(const TopEntry *a, const TopEntry *b) {
	// return TRUE if a ranks higher than b
	if (a->total > b->total) return TRUE;
	if (a->total < b->total) return FALSE;
	const char *da = get_passageiro_document_number(a->p);
	const char *db = get_passageiro_document_number(b->p);
	return doc_compare(da, db) < 0;
}

static void top10_insert(GArray *top, TopEntry e) {
	// top sorted by (total desc, doc asc)
	guint len = top->len;
	guint pos = 0;
	for (; pos < len; pos++) {
		TopEntry *cur = &g_array_index(top, TopEntry, pos);
		if (entry_better(&e, cur)) break;
	}
	g_array_insert_val(top, pos, e);
	if (top->len > 10) {
		g_array_set_size(top, 10);
	}
}

static gint compare_gint64(gconstpointer a, gconstpointer b) {
	gint64 x = *(const gint64*)a;
	gint64 y = *(const gint64*)b;
	return (x > y) - (x < y);
}

static void build_cache_if_needed(SistemaGestor *gestor) {
	if (q4_cache && q4_cache->built) return;
	if (!q4_cache) {
		q4_cache = g_new0(Q4Cache, 1);
		if (!q4_cache) return;
		q4_cache->week_to_top10 = g_hash_table_new_full(g_int64_hash, g_int64_equal, g_free, free_top10_array);
		q4_cache->sorted_week_starts = g_array_new(FALSE, FALSE, sizeof(gint64));
		q4_cache->built = FALSE;
		if (!q4_cache->week_to_top10 || !q4_cache->sorted_week_starts) {
			querie4_cleanup_cache();
			return;
		}
	}
	if (!gestor) return;

	ReservaPtrManager *reservas_m = gestor_global_get_reservas(gestor);
	VooPtrManager *voos_m = gestor_global_get_voos(gestor);
	PassageiroPtrManager *pass_m = gestor_global_get_passageiros(gestor);
	if (!reservas_m || !voos_m || !pass_m) {
		q4_cache->built = TRUE;
		return;
	}

	// week_totals: week_start_day -> (passageiro* -> double*)
	GHashTable *week_totals = g_hash_table_new_full(
		g_int64_hash,
		g_int64_equal,
		g_free,
		(GDestroyNotify)g_hash_table_destroy
	);
	if (!week_totals) {
		q4_cache->built = TRUE;
		return;
	}

	guint total_reservas = reserva_ptr_manager_count(reservas_m);
	for (guint i = 0; i < total_reservas; i++) {
		const Reserva *r = reserva_ptr_manager_get(reservas_m, i);
		if (!r) continue;

		const char *doc = reserva_get_document_number(r);
		if (!doc || doc[0] == '\0') continue;
		Passageiro *p = passageiro_ptr_manager_find_by_doc(pass_m, doc);
		if (!p) continue;

		const char *fid = reserva_get_flight_id_at(r, 0);
		if (!fid || fid[0] == '\0') continue;
		const Voo *v = voo_ptr_manager_find_by_id(voos_m, fid);
		if (!v) continue;

		gint64 day = (gint64)voo_get_departure_day_number(v);
		gint64 wstart = week_start_sunday(day);

		gint64 tmp = wstart;
		GHashTable *pass_to_sum = g_hash_table_lookup(week_totals, &tmp);
		if (!pass_to_sum) {
			gint64 *k = g_new(gint64, 1);
			if (!k) continue;
			*k = wstart;
			pass_to_sum = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
			if (!pass_to_sum) {
				g_free(k);
				continue;
			}
			g_hash_table_insert(week_totals, k, pass_to_sum);
		}

		double *sum = g_hash_table_lookup(pass_to_sum, p);
		if (!sum) {
			sum = g_new0(double, 1);
			if (!sum) continue;
			g_hash_table_insert(pass_to_sum, p, sum);
		}
		*sum += reserva_get_price(r);
	}

	// Construir top10 por semana e libertar totais.
	GHashTableIter iter;
	gpointer key = NULL;
	gpointer value = NULL;
	g_hash_table_iter_init(&iter, week_totals);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		gint64 wstart = *(gint64*)key;
		GHashTable *pass_to_sum = (GHashTable*)value;
		if (!pass_to_sum) continue;

		GArray *top = g_array_new(FALSE, FALSE, sizeof(TopEntry));
		if (!top) continue;

		GHashTableIter it2;
		gpointer pkey = NULL;
		gpointer pval = NULL;
		g_hash_table_iter_init(&it2, pass_to_sum);
		while (g_hash_table_iter_next(&it2, &pkey, &pval)) {
			Passageiro *p = (Passageiro*)pkey;
			double total = pval ? *(double*)pval : 0.0;
			TopEntry e = {.p = p, .total = total};
			if (top->len < 10) {
				top10_insert(top, e);
			} else {
				TopEntry *worst = &g_array_index(top, TopEntry, top->len - 1);
				if (entry_better(&e, worst)) {
					top10_insert(top, e);
				}
			}
		}

		GPtrArray *top10 = g_ptr_array_new();
		if (top10) {
			for (guint j = 0; j < top->len; j++) {
				TopEntry *e = &g_array_index(top, TopEntry, j);
				g_ptr_array_add(top10, e->p);
			}
			gint64 *wk = g_new(gint64, 1);
			if (wk) {
				*wk = wstart;
				g_hash_table_insert(q4_cache->week_to_top10, wk, top10);
				g_array_append_val(q4_cache->sorted_week_starts, wstart);
			} else {
				g_ptr_array_free(top10, TRUE);
			}
		}

		g_array_free(top, TRUE);
	}

	g_hash_table_destroy(week_totals);

	if (q4_cache->sorted_week_starts->len > 1) {
		g_array_sort(q4_cache->sorted_week_starts, compare_gint64);
	}
	q4_cache->built = TRUE;
}

static gboolean doc_is_lower(const char *a, const char *b) {
	return doc_compare(a, b) < 0;
}

void execQ4(char linha[], SistemaGestor *gestor, int *Nout) {
	if (!linha || !gestor || !Nout) return;
	if (linha[0] != '4') return;

	char caminho_output[256];
	snprintf(caminho_output, sizeof(caminho_output), "resultados/command%d_output.txt", *Nout);

	char sep = ';';
	const char *cursor = skip_spaces_const(linha + 1);
	if (!cursor) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}
	if ((*cursor == 'S' || *cursor == 's') && (cursor[1] == '\0' || g_ascii_isspace(cursor[1]))) {
		sep = '=';
		cursor = skip_spaces_const(cursor + 1);
	}

	gboolean has_range = FALSE;
	gint64 begin_day = 0;
	gint64 end_day = 0;
	if (cursor && *cursor) {
		char braw[16] = {0};
		char eraw[16] = {0};
		if (sscanf(cursor, "%15s %15s", braw, eraw) == 2) {
			if (!parse_date_ymd(braw, &begin_day) || !parse_date_ymd(eraw, &end_day) || begin_day > end_day) {
				if (write_blank_line(caminho_output)) (*Nout)++;
				return;
			}
			has_range = TRUE;
		}
	}

	build_cache_if_needed(gestor);

	if (!q4_cache || !q4_cache->built || !q4_cache->week_to_top10) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}

	gint64 start_week = 0;
	gint64 end_week = 0;
	if (has_range) {
		start_week = week_start_sunday(begin_day);
		end_week = week_start_sunday(end_day);
	} else {
		if (!q4_cache->sorted_week_starts || q4_cache->sorted_week_starts->len == 0) {
			if (write_blank_line(caminho_output)) (*Nout)++;
			return;
		}
		start_week = g_array_index(q4_cache->sorted_week_starts, gint64, 0);
		end_week = g_array_index(q4_cache->sorted_week_starts, gint64, q4_cache->sorted_week_starts->len - 1);
	}

	GHashTable *counts = g_hash_table_new(g_direct_hash, g_direct_equal);
	if (!counts) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}

	for (gint64 w = start_week; w <= end_week; w += 7) {
		gint64 tmp = w;
		GPtrArray *top10 = g_hash_table_lookup(q4_cache->week_to_top10, &tmp);
		if (!top10) continue;
		for (guint i = 0; i < top10->len; i++) {
			Passageiro *p = (Passageiro*)g_ptr_array_index(top10, i);
			if (!p) continue;
			int cur = GPOINTER_TO_INT(g_hash_table_lookup(counts, p));
			cur++;
			g_hash_table_replace(counts, p, GINT_TO_POINTER(cur));
		}
	}

	Passageiro *best = NULL;
	int best_count = 0;
	GHashTableIter it;
	gpointer k = NULL;
	gpointer v = NULL;
	g_hash_table_iter_init(&it, counts);
	while (g_hash_table_iter_next(&it, &k, &v)) {
		Passageiro *p = (Passageiro*)k;
		int c = GPOINTER_TO_INT(v);
		if (c > best_count) {
			best = p;
			best_count = c;
		} else if (c == best_count && c > 0) {
			const char *da = best ? get_passageiro_document_number(best) : NULL;
			const char *db = p ? get_passageiro_document_number(p) : NULL;
			if (!best || doc_is_lower(db, da)) {
				best = p;
				best_count = c;
			}
		}
	}

	FILE *out = fopen(caminho_output, "w");
	if (!out) {
		g_hash_table_destroy(counts);
		return;
	}

	if (!best || best_count <= 0) {
		fputc('\n', out);
		fclose(out);
		g_hash_table_destroy(counts);
		(*Nout)++;
		return;
	}

	const char *doc = get_passageiro_document_number(best);
	const char *fn = passageiro_get_first_name(best);
	const char *ln = passageiro_get_last_name(best);
	const char *dob = passageiro_get_dob(best);
	const char *nat = passageiro_get_nationality(best);
	if (!doc) doc = "";
	if (!fn) fn = "";
	if (!ln) ln = "";
	if (!dob) dob = "";
	if (!nat) nat = "";

	fprintf(out, "%s%c%s%c%s%c%s%c%s%c%d\n", doc, sep, fn, sep, ln, sep, dob, sep, nat, sep, best_count);
	fclose(out);

	g_hash_table_destroy(counts);
	(*Nout)++;
}
