#include "parser_voo.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gestor_aeronave.h"
#include "gestor_voo.h"
#include "parser_aeroporto.h"


static int parse_horario(const char *s, Horario *h) {
    if (!s || !h) return 0;
    int Y, M, D, hh, mm;
    if (sscanf(s, "%d-%d-%d %d:%d", &Y, &M, &D, &hh, &mm) != 5) return 0;
    if (Y < 1900 || M < 1 || M > 12 || D < 1 || D > 31 || hh < 0 || hh > 23 || mm < 0 || mm > 59) return 0;
    set_horario(h, Y, M, D, hh, mm);
    return 1;
}



static int parse_horario_optional(const char *s, Horario *h, int *tem_valor) {
    if (!h || !tem_valor || !s) return 0;
    if (s[0] == '\0') return 0;
    if (g_ascii_strcasecmp(s, "N/A") == 0) {
        horario_reset(h);
        *tem_valor = 0;
        return 1;
    }
    *tem_valor = 1;
    return parse_horario(s, h);
}

static Status parse_status(const char *s, int *ok) {
    *ok = 1;
    if (!s) { *ok = 0; return ON_TIME; }
    if (g_ascii_strcasecmp(s, "ON_TIME") == 0 || g_ascii_strcasecmp(s, "ON TIME") == 0 || g_ascii_strcasecmp(s, "On time") == 0) return ON_TIME;
    if (g_ascii_strcasecmp(s, "DELAYED") == 0 || g_ascii_strcasecmp(s, "Delayed") == 0) return DELAYED;
    if (g_ascii_strcasecmp(s, "CANCELLED") == 0 || g_ascii_strcasecmp(s, "Cancelled") == 0) return CANCELLED;
    *ok = 0; return ON_TIME;
}

static int validar_url_http(const char *s) {
    if (!s) return 0;
    return g_str_has_prefix(s, "http://") || g_str_has_prefix(s, "https://");
}

ParseResultado* parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    (void)gestor;

    if (n_fields < 12) {
        return r;
    }

    const char *flight_id   = fields[0];
    const char *dep_str     = fields[1];
    const char *act_dep_str = fields[2];
    const char *arr_str     = fields[3];
    const char *act_arr_str = fields[4];
    const char *gate_str    = fields[5];
    const char *status_str  = fields[6];
    const char *origem      = fields[7];
    const char *destino     = fields[8];
    const char *aircraft    = fields[9];
    const char *airline     = fields[10];
    const char *url         = fields[11];

    if (!validar_flight_id(flight_id)) {
        return r;
    }
    if (!validar_iata(origem) || !validar_iata(destino)) {
        return r;
    }
    if (strcmp(origem, destino) == 0) {
        return r;
    }
    if (!aircraft  || !existe_aeronave(gestor_global_get_aeronaves(gestor), aircraft)) {
        return r;
    }
    if (!airline ) {
        return r;
    }

    char *endp = NULL;
    long gate = strtol(gate_str ? gate_str : "", &endp, 10);
    if (!gate_str || endp == gate_str || gate <= 0) {
        return r;
    }

    Horario *dep = horario_new();
    Horario *act_dep = horario_new();
    Horario *arr = horario_new();
    Horario *act_arr = horario_new();

    if (!parse_horario(dep_str, dep)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    if (!parse_horario(arr_str, arr)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    if (comparar_horarios(arr, dep) <= 0) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    int tem_act_dep = 0;
    int tem_act_arr = 0;
    if (!parse_horario_optional(act_dep_str, act_dep, &tem_act_dep)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    if (!parse_horario_optional(act_arr_str, act_arr, &tem_act_arr)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    
    int ok_status = 0;
    Status st = parse_status(status_str, &ok_status);
    if (!ok_status) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr); 
        horario_free(act_arr);
        return r;
    }


    if (st == CANCELLED) {
        if (tem_act_dep || tem_act_arr) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return r;
        }
    }
    
    else if (st == DELAYED) {
        if (tem_act_dep && comparar_horarios(act_dep, dep) < 0) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return r; // Partiu ANTES da hora
        }
        if (tem_act_arr && comparar_horarios(act_arr, arr) < 0) {
            horario_free(dep);
            horario_free(act_dep);
            horario_free(arr);
            horario_free(act_arr);
            return r; // Chegou ANTES da hora
        }
    }
    

    if (!tem_act_dep) horario_copy(act_dep, dep);
    if (!tem_act_arr) horario_copy(act_arr, arr);
    if (tem_act_dep && tem_act_arr && comparar_horarios(act_arr, act_dep) <= 0) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }

    if (!validar_url_http(url)) {
        horario_free(dep);
        horario_free(act_dep);
        horario_free(arr);
        horario_free(act_arr);
        return r;
    }
    
    Voo *v = voo_create(flight_id, dep, act_dep, arr, act_arr, (int)gate, st, origem, destino, aircraft, airline, url);
    horario_free(dep);
    horario_free(act_dep);
    horario_free(arr);
    horario_free(act_arr);
    if (!v) {
        return r;
    }

    parse_resultado_mark_valid(r, v);
    return r;
}

int csv_parser_voos(
    const char *caminho_csv,       // Caminho do ficheiro a ler
    const char *ficheiro_erros,    // Caminho para o ficheiro de erros
    void *gestor,     
    SistemaGestor *gestorGlob,             // Ponteiro para o gestor       // Função que faz o parsing da linha
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
        ParseResultado *resultado = parse_voo_line(fields, n_fields, gestorGlob);

        if (!resultado || !parse_resultado_is_valid(resultado)) {
            // Cria o ficheiro de erros se ainda não existir
            if (!ficheiro_erros_ptr) {
                ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
                fwrite(header, 1, strlen(header), ficheiro_erros_ptr);
                if (!ficheiro_erros_ptr) {
                    fputs("Erro ao criar ficheiro de erros: ", stderr);
                    fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
                    fputc('\n', stderr);
                    if (resultado) parse_resultado_destroy(resultado);
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
            if (resultado) parse_resultado_destroy(resultado);
        } else {
            // Adiciona entidade válida ao gestor
            voo_ptr_manager_add_from_ptr_adapter(gestor, parse_resultado_get_entidade(resultado));
            contador_validos++;
            parse_resultado_destroy(resultado);
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