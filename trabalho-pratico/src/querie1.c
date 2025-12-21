// Query 1: Resumo do aeroporto dado o código IATA
#include "querie1.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "aeroportos.h"
#include "gestor_aeroporto.h"
#include "gestor_reserva.h"
#include "gestor_voo.h"
#include "gestorglobal.h"
#include "reservas.h"
#include "voos.h"

// remove espaços no início e fim (in-place)
static char *trim_spaces(char *s) {
    if (!s) return s;
    while (g_ascii_isspace(*s)) s++;
    char *end = s + strlen(s);
    while (end > s && g_ascii_isspace(*(end - 1))) *(--end) = '\0';
    return s;
}

static char *skip_spaces(char *s) {
    if (!s) return NULL;
    while (*s && g_ascii_isspace(*s)) s++;
    return s;
}

// Extrai até max_ids flight ids de uma string no formato "['AB12345', 'CD67890']"
// Nota: fase 2 tem flight ids com 7 ou 8 caracteres.
static int extract_flight_ids(const char *src, char ids[][9], int max_ids) {
    if (!src || max_ids <= 0) return 0;
    char *copy = g_strdup(src);
    if (!copy) return 0;

    char *s = trim_spaces(copy);
    if (*s == '[') {
        size_t L = strlen(s);
        if (L > 0 && s[L - 1] == ']') { s[L - 1] = '\0'; s++; }
    }

    int count = 0;
    for (char *tok = strtok(s, ","); tok && count < max_ids; tok = strtok(NULL, ",")) {
        tok = trim_spaces(tok);
        size_t lt = strlen(tok);
        if (lt >= 2 && ((tok[0] == '\'' && tok[lt - 1] == '\'') || (tok[0] == '"' && tok[lt - 1] == '"'))) {
            tok[lt - 1] = '\0'; tok++; lt -= 2;
        }
        tok = trim_spaces(tok);
        if (lt >= 8) tok[8] = '\0';
        g_strlcpy(ids[count], tok, 9);
        count++;
    }

    g_free(copy);
    return count;
}

static void contar_passageiros(const char *codigo_iata,
                               SistemaGestor *gestor,
                               long *aterragens,
                               long *partidas) {
    if (!codigo_iata || !gestor || !aterragens || !partidas) return;

    ReservaPtrManager *reservas_manager = gestor_global_get_reservas(gestor);
    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    if (!reservas_manager || !voos_manager) return;

    guint total_reservas = reserva_ptr_manager_count(reservas_manager);
    for (guint i = 0; i < total_reservas; i++) {
        const Reserva *reserva = reserva_ptr_manager_get(reservas_manager, i);
        if (!reserva) continue;

        const char *flight_ids_raw = reserva_get_flight_ids(reserva);
        if (!flight_ids_raw || flight_ids_raw[0] == '\0') continue;

        char ids[2][9] = {{0}};
        int n_ids = extract_flight_ids(flight_ids_raw, ids, 2);
        if (n_ids <= 0) continue;

        for (int j = 0; j < n_ids; j++) {
            const Voo *voo = voo_ptr_manager_find_by_id(voos_manager, ids[j]);
            if (!voo) continue;
            if (voo_get_status(voo) == CANCELLED) continue;

            const char *origem = voo_get_iata_origem(voo);
            const char *destino = voo_get_iata_destino(voo);

            if (origem && strcmp(origem, codigo_iata) == 0) (*partidas)++;
            if (destino && strcmp(destino, codigo_iata) == 0) (*aterragens)++;
        }
    }
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
        contar_passageiros(codigo_iata, gestor, &aterragem, &partida);

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