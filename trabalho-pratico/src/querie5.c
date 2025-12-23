
#include "querie5.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "gestorglobal.h"
#include "gestor_voo.h"

typedef struct {
	const AirlineDelayStats *stats;
	double avg_rounded_3;
} AirlineDelayView;

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

static double round3(double x) {
	// arredondamento a 3 casas sem libm
	long long scaled = (long long)(x * 1000.0 + (x >= 0.0 ? 0.5 : -0.5));
	return (double)scaled / 1000.0;
}

static gint compare_delay_views(gconstpointer a, gconstpointer b) {
	const AirlineDelayView *lhs = *((AirlineDelayView * const *)a);
	const AirlineDelayView *rhs = *((AirlineDelayView * const *)b);
	if (!lhs || !rhs) return 0;
	if (lhs->avg_rounded_3 < rhs->avg_rounded_3) return 1;
	if (lhs->avg_rounded_3 > rhs->avg_rounded_3) return -1;
	const char *al = lhs->stats && lhs->stats->airline ? lhs->stats->airline : "";
	const char *ar = rhs->stats && rhs->stats->airline ? rhs->stats->airline : "";
	return strcmp(al, ar);
}

static GPtrArray *cached_sorted_views = NULL; // AirlineDelayView*

static void build_cached_sorted_views(VooPtrManager *voos_manager) {
	if (cached_sorted_views) return;
	if (!voos_manager) {
		cached_sorted_views = g_ptr_array_new();
		return;
	}
	GHashTable *stats = voo_ptr_manager_get_delay_stats_by_airline(voos_manager);
	cached_sorted_views = g_ptr_array_new();
	if (!stats || !cached_sorted_views) return;

	GHashTableIter iter;
	gpointer key = NULL, value = NULL;
	g_hash_table_iter_init(&iter, stats);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		const AirlineDelayStats *s = (const AirlineDelayStats*)value;
		if (!s || !s->airline || s->airline[0] == '\0') continue;
		if (s->delayed_flights_count <= 0) continue;
		double avg = (double)s->total_delay_minutes / (double)s->delayed_flights_count;
		AirlineDelayView *view = g_new0(AirlineDelayView, 1);
		if (!view) continue;
		view->stats = s;
		view->avg_rounded_3 = round3(avg);
		g_ptr_array_add(cached_sorted_views, view);
	}

	g_ptr_array_sort(cached_sorted_views, compare_delay_views);
}

void execQ5(char linha[], SistemaGestor *gestor, int *Nout) {
	if (!linha || !gestor || !Nout) return;
	if (linha[0] != '5') return;

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

	char *endptr = NULL;
	long parsed = strtol(cursor ? cursor : "", &endptr, 10);
	if (!endptr || endptr == cursor || parsed <= 0) {
		if (write_blank_line(caminho_output)) (*Nout)++;
		return;
	}
	if (parsed > INT_MAX) parsed = INT_MAX;
	int N = (int)parsed;

	VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
	build_cached_sorted_views(voos_manager);

	FILE *out = fopen(caminho_output, "w");
	if (!out) return;

	if (!cached_sorted_views || cached_sorted_views->len == 0) {
		fputc('\n', out);
		fclose(out);
		(*Nout)++;
		return;
	}

	int max = N;
	if (max > (int)cached_sorted_views->len) max = (int)cached_sorted_views->len;
	for (int i = 0; i < max; i++) {
		AirlineDelayView *view = g_ptr_array_index(cached_sorted_views, i);
		if (!view || !view->stats) continue;
		const char *airline = view->stats->airline ? view->stats->airline : "";
		fprintf(out, "%s%c%ld%c%.3f\n",
				airline,
				sep,
				view->stats->delayed_flights_count,
				sep,
				view->avg_rounded_3);
	}

	fclose(out);
	(*Nout)++;
}
