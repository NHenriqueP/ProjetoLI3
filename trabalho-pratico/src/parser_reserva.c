#include "parser_reserva.h"

#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gestorglobal.h"
#include "gestor_voo.h"
#include "voos.h"
#include "gestor_passageiro.h"
#include "csv_parser.h"

static int validar_reservation_id(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    if (s[0] != 'R') return 0;
    for (size_t i = 1; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

static int validar_seat(const char *s) {
    if (!s || strlen(s) < 2) return 0;
    int i = 0;
    if (!isdigit((unsigned char)s[i])) return 0;
    while (s[i] != '\0' && isdigit((unsigned char)s[i])) {
        i++;
    }
    if (s[i] == '\0' || !isalpha((unsigned char)s[i])) return 0;
    i++;
    if (s[i] != '\0') return 0;
    return 1;
}

static int validar_preco(const char *s, double *out) {
    if (!s || !out) return 0;
    char *endptr;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return 0;
    if (val < 0) return 0;
    *out = val;
    return 1;
}

static int parse_boolean(const char *s, bool *out) {
    if (!s || !out) return 0;
    if (g_ascii_strcasecmp(s, "true") == 0 || strcmp(s, "1") == 0) { *out = true; return 1; }
    if (g_ascii_strcasecmp(s, "false") == 0 || strcmp(s, "0") == 0) { *out = false; return 1; }
    return 0;
}

static int existe_flight_id(const char *id, SistemaGestor *gestor) {
    VooPtrManager *voos_manager = gestor_global_get_voos(gestor);
    if (!voos_manager) return 0;
    return voo_ptr_manager_find_by_id(voos_manager, id) != NULL;
}

static char* trim_spaces(char *s) {
    if (!s) return s;
    while (g_ascii_isspace(*s)) s++;
    char *end = s + strlen(s);
    while (end > s && g_ascii_isspace(*(end - 1))) *(--end) = '\0';
    return s;
}

static int extrair_ids_de_lista(const char *src, char ids[][8], int max_ids) {
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

static int processar_e_validar_flights(const char *flight_ids_str, SistemaGestor *gestor) {
    if (!flight_ids_str || *flight_ids_str == '\0') return 0;
    char ids[2][8] = {{0}};
    int n = extrair_ids_de_lista(flight_ids_str, ids, 2);
    if (n < 1 || n > 2) return 0;
    for (int i = 0; i < n; i++) {
        if (!validar_flight_id(ids[i]) || !existe_flight_id(ids[i], gestor)) return 0;
    }
    if (n == 2) {
        const Voo *primeiro = voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[0]);
        const Voo *segundo = voo_ptr_manager_find_by_id(gestor_global_get_voos(gestor), ids[1]);
        if (!primeiro || !segundo) return 0;
        const char *destino = voo_get_iata_destino(primeiro);
        const char *origem = voo_get_iata_origem(segundo);
        if (!destino || !origem) return 0;
        return strcmp(origem, destino) == 0;
    }
    return 1;
}

ParseResultado parse_reserva_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;

    if (n_fields < 8) {
        return r;
    }

    char *reservation_id    = fields[0];
    char *flight_ids        = fields[1];
    char *document_number   = fields[2];
    char *seat              = fields[3];
    char *price_str         = fields[4];
    char *extra_luggage_str = fields[5];
    char *priority_str      = fields[6];
    char *qr_code           = fields[7];

    if (!validar_reservation_id(reservation_id)) {
        return r;
    }

    if (!processar_e_validar_flights(flight_ids, gestor)) {
        return r;
    }

    if (!document_number || strlen(document_number) == 0 || strlen(document_number) > 9 || !existe_document_number(gestor_global_get_passageiros(gestor), document_number)) {
        return r;
    }
    for (const char *p = document_number; *p; ++p) {
        if (!isdigit((unsigned char)*p)) return r;
    }

    if (!validar_seat(seat)) {
        return r;
    }

    double price;
    if (!validar_preco(price_str, &price)) {
        return r;
    }

    bool extra_luggage;
    if (!parse_boolean(extra_luggage_str, &extra_luggage)) {
        return r;
    }

    bool priority_boarding;
    if (!parse_boolean(priority_str, &priority_boarding)) {
        return r;
    }

    Reserva *res = g_new0(Reserva, 1);
    if (!res) {
        return r;
    }

    res->reservation_id = g_strdup(reservation_id);
    res->flight_ids = g_strdup(flight_ids);
    res->document_number = g_strdup(document_number);
    res->seat = g_strdup(seat);
    res->price = price;
    res->extra_luggage = extra_luggage;
    res->priority_boarding = priority_boarding;
    res->qr_code = g_strdup(qr_code ? qr_code : "");

    if (!res->reservation_id || !res->flight_ids || !res->document_number || !res->seat || !res->qr_code) {
        g_free(res->reservation_id);
        g_free(res->flight_ids);
        g_free(res->document_number);
        g_free(res->seat);
        g_free(res->qr_code);
        g_free(res);
        return r;
    }

    r.valido = true;
    r.entidade = res;
    return r;
}

int csv_parser_reservas(
    const char *caminho_csv,       // Caminho do ficheiro a ler
    const char *ficheiro_erros,    // Caminho para o ficheiro de erros
    void *gestor,    
    SistemaGestor *gestorGlob,              // Ponteiro para o gestor       // Função que faz o parsing da linha
    char separador                 // Separador CSV: ','
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(caminho_csv ? caminho_csv : "<NULL>", stderr);
        fputs("': ", stderr);//fecha a aspa e separa.
        fputs(strerror(errno), stderr);
        fputc('\n', stderr);
        return 0;
    }

    FILE *ficheiro_erros_ptr = NULL;
    char *linha = NULL;
    // getline() usa size_t para o tamanho  por isso o uso do size_t 
    size_t tam = 0;
    // 'lida' guarda o resultado de getline(): número de bytes lidos ou -1 em EOF/erro.
    // getline() retorna ssize_t (signed) para poder indicar -1, por isso usamos ssize_t.
    ssize_t lida;
    char *header = NULL;
    bool header_lido = false;
    int contador_validos = 0;

    while ((lida = getline(&linha, &tam, ficheiro)) != -1) {
        // Preservar a linha original (incluindo terminadores de linha) para o ficheiro de erros
        size_t original_len = (size_t)lida;
        char *linha_original = g_strndup(linha, original_len);

        if (header_lido) {
        // Remover \n e \r para efeitos de parsing (sem afetar o comprimento usado para escrever no ficheiro de erros)
        while (lida > 0 && (linha[lida - 1] == '\n' || linha[lida - 1] == '\r')) // \r serve para compatibilidade com Windows
            linha[--lida] = '\0';
        }
        // Guardar o cabeçalho (primeira linha) e continuar
        if (!header_lido) {
            header = g_strdup(linha);
            header_lido = true;
            // já não precisamos da cópia da linha original nesta iteração
            g_free(linha_original);
            continue;
        }

        // Dividir linha em campos (suporta aspas e vírgulas dentro de aspas)
        char *fields[15];
        int max_fields = 15;
        int n_fields = csv_split_quoted(linha, separador, fields, max_fields);

        // Validar linha com o parser específico fornecido
        ParseResultado resultado = parse_reserva_line(fields, n_fields, gestorGlob);

        if (!resultado.valido) {
            // Cria o ficheiro de erros se ainda não existir
            if (!ficheiro_erros_ptr) {
                ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
                fwrite(header, 1, strlen(header), ficheiro_erros_ptr);
                if (!ficheiro_erros_ptr) {
                    fputs("Erro ao criar ficheiro de erros: ", stderr);
                    fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
                    fputc('\n', stderr);
                    g_free(linha_original);
                    break;
                }
            }
            // Escreve a linha inválida exatamente como lida (com terminadores originais)
            if (linha_original) {
                fwrite(linha_original, 1, original_len, ficheiro_erros_ptr);
            } else {
                // fallback: escreve versão já normalizada
                fputs(linha, ficheiro_erros_ptr);
                fputc('\n', ficheiro_erros_ptr);
            }
        } else {
            // Adiciona entidade válida ao gestor
            reserva_ptr_manager_add_from_ptr_adapter(gestor, resultado.entidade);
            contador_validos++;
        }

        // libertar cópia da linha
        g_free(linha_original);
    }

    // Fechar e libertar tudo
    if (ficheiro_erros_ptr) fclose(ficheiro_erros_ptr);
    g_free(header);
    free(linha);
    fclose(ficheiro);

    return contador_validos;
}