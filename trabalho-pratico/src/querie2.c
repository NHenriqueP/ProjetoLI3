// Query 2: Top N aeronaves com mais voos realizados (exclui CANCELLED)
// Opcional: filtro por fabricante
// !!!!!!!!!!!!!!!!!!!AINDA ESTA A SER FEITO, IGNORAR POR ENQUANTO!!!!!!!!!!!!!!!!!!!!!!!!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aeronaves.h"
#include "voos.h"

// Resultado que podemos imprimir ou devolver
typedef struct {
	char id[10];           // identificador da aeronave (até 9 + '\0')
	char fabricante[15];   // fabricante
	char modelo[15];       // modelo
	int total_voos;        // voos realizados (estado != CANCELLED)
} Q2Result;

// Estrutura interna para mapear id -> índice e contador
typedef struct {
	char id[10];
	size_t idx_aeronave; // índice no array de aeronaves
	int total_voos;
} IdIndex;

static int cmp_id_asc(const void *a, const void *b) {
	const IdIndex *ia = (const IdIndex *)a;
	const IdIndex *ib = (const IdIndex *)b;
	return strcmp(ia->id, ib->id);
}

static int cmp_result_rank(const void *a, const void *b) {
	const Q2Result *ra = (const Q2Result *)a;
	const Q2Result *rb = (const Q2Result *)b;
	// Ordenação: total_voos desc, id asc
	if (ra->total_voos > rb->total_voos) return -1;
	if (ra->total_voos < rb->total_voos) return 1;
	return strcmp(ra->id, rb->id);
}

// Procura binária por id na tabela ordenada por id
static IdIndex *idindex_bsearch(IdIndex *base, size_t n, const char *key) {
	size_t left = 0;
	size_t right = (n == 0) ? 0 : n - 1;
	while (n != 0 && left <= right) {
		size_t mid = left + (right - left) / 2;
		int c = strcmp(base[mid].id, key);
		if (c == 0) return &base[mid];
		if (c < 0) left = mid + 1; else {
			if (mid == 0) break;
			right = mid - 1;
		}
	}
	return NULL;
}

// API principal de Query 2
// - aeronaves/na: catálogo de aeronaves
// - voos/nv: lista de voos
// - topN: número de linhas a produzir
// - fabricante_filter: opcional (NULL ou "" para ignorar filtro)
// - out: ficheiro de saída (formato CSV: id,fabricante,modelo,total_voos)
void querie2_top_n_aeronaves(const Aeronave *aeronaves, size_t na,
							 const Voo *voos, size_t nv,
							 size_t topN, const char *fabricante_filter,
							 FILE *out)
{
	if (out == NULL) return;
	if (aeronaves == NULL || voos == NULL) return;
	if (na == 0 || nv == 0 || topN == 0) return;

	// Normalizar filtro (se vazio tratar como sem filtro)
	int usar_filtro = 0;
	if (fabricante_filter != NULL) {
		if (fabricante_filter[0] != '\0') usar_filtro = 1;
	}

	// 1) Construir tabela de ids de aeronaves (com filtro de fabricante se existir)
	IdIndex *table = (IdIndex *)malloc(sizeof(IdIndex) * na);
	if (table == NULL) return;
	size_t tcount = 0;

	for (size_t i = 0; i < na; i++) {
		int passa = 1;
		if (usar_filtro) {
			// Comparação exata do fabricante
			if (strcmp(aeronaves[i].fabricante, fabricante_filter) != 0) passa = 0;
		}
		if (passa) {
			// Copiar id e inicializar
			memset(table[tcount].id, 0, sizeof(table[tcount].id));
			strncpy(table[tcount].id, aeronaves[i].id, sizeof(table[tcount].id) - 1);
			table[tcount].idx_aeronave = i;
			table[tcount].total_voos = 0;
			tcount++;
		}
	}

	if (tcount == 0) {
		free(table);
		return; // nada a fazer
	}

	// 2) Ordenar tabela por id para permitir bsearch
	qsort(table, tcount, sizeof(IdIndex), cmp_id_asc);

	// 3) Percorrer voos e acumular contagens (exclui CANCELLED)
	for (size_t j = 0; j < nv; j++) {
		if (voos[j].estado == CANCELLED) continue;
		// procurar aircraft id
		char key[10];
		memset(key, 0, sizeof(key));
		strncpy(key, voos[j].aircraft, sizeof(key) - 1);
		IdIndex *entry = idindex_bsearch(table, tcount, key);
		if (entry != NULL) {
			entry->total_voos += 1;
		}
	}

	// 4) Construir vetor de resultados
	Q2Result *res = (Q2Result *)malloc(sizeof(Q2Result) * tcount);
	if (res == NULL) {
		free(table);
		return;
	}
	size_t rcount = 0;
	for (size_t k = 0; k < tcount; k++) {
		// Podemos incluir mesmo os que têm 0 voos; se quiser só >0, descomentar filtro
		// if (table[k].total_voos == 0) continue;
		const Aeronave *a = &((aeronaves)[table[k].idx_aeronave]);
		memset(res[rcount].id, 0, sizeof(res[rcount].id));
		strncpy(res[rcount].id, a->id, sizeof(res[rcount].id) - 1);
		memset(res[rcount].fabricante, 0, sizeof(res[rcount].fabricante));
		strncpy(res[rcount].fabricante, a->fabricante, sizeof(res[rcount].fabricante) - 1);
		memset(res[rcount].modelo, 0, sizeof(res[rcount].modelo));
		strncpy(res[rcount].modelo, a->modelo, sizeof(res[rcount].modelo) - 1);
		res[rcount].total_voos = table[k].total_voos;
		rcount++;
	}

	// 5) Ordenar resultados por total desc e id asc (empates)
	qsort(res, rcount, sizeof(Q2Result), cmp_result_rank);

	// 6) Emissão do top N em CSV: id,fabricante,modelo,total_voos
	size_t emit = topN;
	if (emit > rcount) emit = rcount;
	for (size_t i = 0; i < emit; i++) {
		// Formato CSV sem espaços, consistente com exemplos do projeto
		// Ajusta a ordem/colunas se o enunciado exigir diferente
		fprintf(out, "%s,%s,%s,%d\n", res[i].id, res[i].fabricante, res[i].modelo, res[i].total_voos);
	}

	free(res);
	free(table);
}