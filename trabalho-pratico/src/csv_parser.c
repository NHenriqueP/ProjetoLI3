#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "csv_parser.h"        
#include "aeroportos.h"    
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>


// Função que adiciona um elemento válido ao gestor
typedef int (*GestorAddFunc)(void *gestor, const void *entidade);





int csv_parser(
    const char *caminho_csv,       // Caminho do ficheiro a ler
    const char *ficheiro_erros,    // Caminho para o ficheiro de erros
    void *gestor,                  // Ponteiro para o gestor
    LinhaParser parser_func,       // Função que faz o parsing da linha
    GestorAddFunc add_func,        // Função que adiciona a entidade válida
    char separador                 // Separador CSV: ','
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fprintf(stderr, "Erro ao abrir ficheiro: %s\n", caminho_csv);
        return 0;
    }

    FILE *ficheiro_erros_ptr = NULL;
    char *linha = NULL;
    int tam = 0;
    int lida;
    char *header = NULL;
    bool header_lido = false;
    int contador_validos = 0;

    while ((lida = getline(&linha, &tam, ficheiro)) != -1) {
        // Remover \n e \r
        while (lida > 0 && (linha[lida - 1] == '\n' || linha[lida - 1] == '\r'))
            linha[--lida] = '\0';

        // Guardar o cabeçalho (primeira linha) e continuar
        if (!header_lido) {
            header = strdup(linha);
            header_lido = true;
            continue;
        }

        // Dividir linha em campos
        char *fields[64];
        int n_fields = 0;
        char *tmp = linha;
        char *token;
        char delim[2] = { separador, '\0' };
        while ((token = strsep(&tmp, delim)) && n_fields < 64) {
            fields[n_fields++] = token;
        }

        // Validar linha com o parser específico fornecido
        ParseResultado resultado = parser_func(fields, n_fields);

        if (!resultado.valido) {
            // Cria o ficheiro de erros se ainda não existir
            if (!ficheiro_erros_ptr) {
                ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
                if (!ficheiro_erros_ptr) {
                    fprintf(stderr, "Erro ao criar ficheiro de erros: %s\n", ficheiro_erros);
                    break;
                }
            }
            // Escreve a linha inválida
            fprintf(ficheiro_erros_ptr, "%s\n", linha);
        } else {
            // Adiciona entidade válida ao gestor
            add_func(gestor, resultado.entidade);
            contador_validos++;
        }
    }

    // Fechar e libertar tudo
    if (ficheiro_erros_ptr) fclose(ficheiro_erros_ptr);
    free(header);
    free(linha);
    fclose(ficheiro);

    return contador_validos;
}


ParseResultado parse_airport_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;
    r.motivo = NULL;

    // Esperamos exatamente 8 campos 
    // Exemplo: IATA, Nome, Cidade, País, Latitude, Longitude, ICAO, Tipo
    if (n_fields < 8) {
        r.motivo = strdup("Número incorreto de campos");
        return r;
    }

    // Extrair e limpar campos
    char *iata      = trim(fields[0]);
    char *nome      = trim(fields[1]);
    char *cidade    = trim(fields[2]);
    char *pais      = trim(fields[3]);
    char *latitude  = trim(fields[4]);
    char *longitude = trim(fields[5]);
    char *icao      = trim(fields[6]);
    char *tipo      = trim(fields[7]);

    
    if (!validar_iata(iata)) {
        r.motivo = ("IATA inválido");
        return r;
    }

    if (!validar_icao(icao)) {
        r.motivo = ("ICAO inválido");
        return r;
    }

    if (!is_number(latitude) || !is_number(longitude)) {
        r.motivo = ("Latitude ou longitude inválida");
        return r;
    }

    if (strlen(nome) == 0 || strlen(cidade) == 0 || strlen(pais) == 0) {
        r.motivo = ("Campos de texto vazios");
        return r;
    }

    if(tipo_aeroporto_valido(tipo)==0) {
        r.motivo= ("tipo do aeroporto inválido");
    }




    
    Aeroporto *a = malloc(sizeof(Aeroporto));
    if (!a) {
        r.motivo = strdup("Falha de memória ao criar aeroporto");
        return r;
    }

    // Atribuir campos (usando strdup)
    a->iata      = strdup(iata);
    a->nome      = strdup(nome);
    a->cidade    = strdup(cidade);
    a->pais      = strdup(pais);
    a->latitude  = strdup(latitude);
    a->longitude = strdup(longitude);
    a->icao      = strdup(icao);
    a->tipo      = tipo;

    // Verificação de alocação
    if (!a->iata || !a->nome || !a->cidade || !a->pais || 
        !a->latitude || !a->longitude || !a->icao) {
        free(a->iata); free(a->nome); free(a->cidade);
        free(a->pais); free(a->latitude); free(a->longitude);
        free(a->icao); free(a);
        r.motivo = strdup("Erro de alocação de memória");
        return r;
    }

    // ======= Sucesso =======
    r.valido = true;
    r.entidade = a;
    return r;
}
// Verifica se a string está no formato correto de coordenada
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