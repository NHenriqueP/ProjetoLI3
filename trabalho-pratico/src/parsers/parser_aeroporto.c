#include "parser_aeroporto.h"
#include <glib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "csv_parser.h"
#include "gestor_aeroporto.h"

int verifica_coordenadas(const char *s, int digitos_max) {
    if (!s || *s == '\0') return 0;
    const char *p = s;
    if (*p == '-') p++;
    const char *int_start = p;
    while (*p && isdigit((unsigned char)*p)) p++;
    size_t int_len = p - int_start;
    if (int_len < 1 || int_len > (size_t)digitos_max) return 0;
    if (*p != '.') return 0;
    p++;
    size_t frac_len = 0;
    while (*p && isdigit((unsigned char)*p)) { frac_len++; p++; if (frac_len > 8) return 0; }
    if (frac_len < 1) return 0;
    if (*p != '\0') return 0;
    return 1;
}

// Valida latitude: formato e intervalo
int validar_latitude(const char *s) {
    if (!verifica_coordenadas(s, 2)) return 0;
    char *end;
    double v = strtod(s, &end);
    if (!(v >= -90.0 && v <= 90.0)) return 0;
    return 1;
}

// Valida longitude: formato e intervalo
int validar_longitude(const char *s) {
    if (!verifica_coordenadas(s, 3)) return 0;
    char *end;
    double v = strtod(s, &end);
    if (!(v >= -180.0 && v <= 180.0)) return 0;
    return 1;
}

int tipo_aeroporto_valido(char *tipo) {
    if (!tipo) return 0;
    return
        strcmp(tipo, "small_airport") == 0 ||
        strcmp(tipo, "medium_airport") == 0 ||
        strcmp(tipo, "large_airport") == 0 ||
        strcmp(tipo, "heliport") == 0 ||
        strcmp(tipo, "seaplane_base") == 0;
}

// Valida se a string é um código IATA (3 letras)
int validar_iata(const char *s) {
    if (!s) return 0;
    int len = (int)strlen(s);
    if (len != 3) return 0;
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (c < 'A' || c > 'Z') return 0; // apenas maiúsculas A-Z
    }
    return 1;
}

// Valida se a string é um código ICAO (4 caracteres alfanuméricos)
int validar_icao(const char *s) {
    if (!s) return 0;
    size_t len = strlen(s);
    if (len != 4) return 0;
    for (size_t i = 0; i < len; i++) {
        if (!isalnum((unsigned char)s[i])) return 0;
    }
    return 1;
}

void* criar_aeroporto(char **fields, int n_fields, void *ctx) {
    (void)ctx;
    if (n_fields < 8) return NULL;

    char *iata      = fields[0];
    char *nome      = fields[1];
    char *cidade    = fields[2];
    char *pais      = fields[3];
    char *latitude  = fields[4];
    char *longitude = fields[5];
    char *icao      = fields[6];
    char *tipo      = fields[7];

    if (!validar_iata(iata) || !validar_icao(icao)) return NULL;
    if (!validar_latitude(latitude) || !validar_longitude(longitude)) return NULL;
    if (!nome || !cidade || !pais || strlen(nome) == 0 || strlen(cidade) == 0 || strlen(pais) == 0) return NULL;
    if (tipo_aeroporto_valido(tipo) == 0) return NULL;

    return aeroporto_create(iata,
                            nome,
                            cidade,
                            pais,
                            tipo);
}

ParseResultado* parse_airport_line(char **fields, int n_fields) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    Aeroporto *a = (Aeroporto*)criar_aeroporto(fields, n_fields, NULL);
    if (!a) return r;

    parse_resultado_mark_valid(r, a);
    return r;
}

int csv_parser_airports(
    const char *caminho_csv,       // Caminho do ficheiro a ler
    const char *ficheiro_erros,    // Caminho para o ficheiro de erros
    void *gestor,                  // Ponteiro para o gestor       // Função que faz o parsing da linha
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
        ParseResultado *resultado = parse_airport_line(fields, n_fields);

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
            airport_ptr_manager_add_from_ptr_adapter(gestor, parse_resultado_get_entidade(resultado));
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