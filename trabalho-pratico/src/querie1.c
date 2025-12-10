// Query 1: Resumo do aeroporto dado o código IATA
#include "querie1.h"
#include <ctype.h>
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

// Extrai até max_ids flight ids de uma string no formato "['AB12345', 'CD67890']"
static int extract_flight_ids(const char *src, char ids[][8], int max_ids) {
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
        if (lt >= 7) tok[7] = '\0';
        g_strlcpy(ids[count], tok, 8);
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

        char ids[2][8] = {{0}};
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
    char codigo_iata[5];

    if (linha[0] != '1' || linha[1] == '\0') {
        // Não é a Query 1 ou a linha está muito curta
        return; 
    }

    char *p = &linha[1]; // Ponto de partida após o '1'

    while (*p != '\0' && *p == ' ') p++; // Ignorar espaços

    if (*p != '\0') {
        size_t len = 0;
        size_t max_len = 4;
        char *ini = p;
        
        while (len < max_len && *p != '\0' && !isspace((unsigned char)*p)) {
            p++;
            len++;
        }
        
        if (len == 3 || len == 4) {
            // strncpy copia 'len' caracteres de 'ini' para 'codigo_iata'
            strncpy(codigo_iata, ini, len);
            
            // Garantir a terminação nula (o strncpy não faz isso automaticamente se len = 4)
            codigo_iata[len] = '\0'; 

            char caminho_output[256]; // Buffer de tamanho suficiente

            snprintf(
                caminho_output,
                sizeof(caminho_output),
                "resultados/command%d_output.txt",
                *Nout
            );

            FILE *output_file = fopen(caminho_output, "w");
            if (!output_file) { 
                return; 
            }

            Aeroporto *aeroporto_ptr = airport_ptr_manager_find_by_iata(
                gestor_global_get_aeroportos(gestor),
                codigo_iata
            );

            if (aeroporto_ptr != NULL) {
                char buf[512];
                long aterragem = 0;
                long partida = 0;
                contar_passageiros(codigo_iata, gestor, &aterragem, &partida);

                if (airport_summary(aeroporto_ptr, buf, sizeof(buf)) > 0) {
                    fprintf(output_file, "%s,%ld,%ld\n", buf, aterragem, partida);
                }
            } else {
                fprintf(output_file,"\n");
            }
            fclose(output_file);
            (*Nout)++;

            return;
        }
    }
}