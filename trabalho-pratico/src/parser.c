#include <stdio.h>
#include <aeroportos.h>
#include <stdbool.h>
#include <stdlib.h>
#include <parser.h>


static int verifica_coordenadas(const char *s, int digitos_max) {
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
    if (!coord_format_ok(s, 2)) {
        return 0;
    }
    char *end;
    double v = strtod(s, &end);
    if (!(v >= -90.0 && v <= 90.0)) {
        return 0;
    }
    return 1;
}
int validar_longitude(const char *s) {
    if (!coord_format_ok(s, 3)) {
        return 0;
    }
    char *end;
    double v = strtod(s, &end);
    if (!(v >= -180.0 && v <= 180.0)) {
        return 0;
    }
    return 0;
}

signed int ler_linha(FILE *f, char **linha, size_t *tam) {
    return getline(linha, tam, f); // retorna nº de caracteres ou -1 no EOF/erro
}

//fazer para voos e aeronaves ou então mudar esta função para geral
int carregarficheiro_aeroporto(AeroportoManager *gestor, const char *caminho_csv) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fprintf(stderr, "Erro ao abrir ficheiro %s\n", caminho_csv);
        return 0;
    }

    // Variáveis auxiliares
    char *linha = NULL;
    int tam = 0;
    signed int lida;
    int contador_validos = 0;
    FILE *ficheiro_erros = NULL;
    char *cabeçalho;
    bool cabeçalho_lido = false;
    FILE *erros = NULL;

    while ((lida = getline(&linha, &tam, ficheiro)) != -1) {
        // Remover newline no fim
        while (lida > 0 && (linha[lida - 1] == '\n' || linha[lida - 1] == '\r')) {
            linha[--lida] = '\0';
        }

        if (!cabeçalho_lido) {
            cabeçalho = strdup(linha);
            cabeçalho_lido = true;
            continue;
        }

        // Dividir linha em campos (aqui usas ',' mas pode ser ';' dependendo do CSV)
        char *fields[16];
        int n_fields = 0;
        char *tmp = linha;
        char *token;
        while ((token = strsep(&tmp, ",")) && n_fields < 16) {
            fields[n_fields++] = token;
        }

        // Parse e valida a linha
        ParseResultado resultado = parse_airport_line(fields, n_fields);

        if (!resultado.valido) {
            // Se ainda não existe ficheiro de erros, cria e escreve o cabeçalho
            if (!erros) {
                escrever_linha_invalida(&ficheiro_erros, linha, cabeçalho);
            }
        } else {
            gestor_adiciona_aeroporto(gestor, resultado.airport);
            contador_validos++;
        }
    }
    if (ficheiro_erros) fclose(ficheiro_erros);
    free(linha);
    free(cabeçalho);
    fclose(ficheiro);

    return contador_validos;
}



ParseResultado parse_airport_line(char **fields, int n_fields) {
    ParseResultado r = { .valido = false, .airport = NULL, .motivo = NULL };

    if (n_fields < 7) {
        r.motivo = "colunas insuficientes"; //verifica o número de colunas minimo esperadas
        return r;
    }

    const char *iata = fields[0]; //Extrai os fields(campos)
    const char *nome = fields[1];
    const char *cidade = fields[2];
    const char *pais = fields[3];
    const char *latitude = fields[4];
    const char *longitude = fields[5];
    const char *icao = fields[6];
    const char *tipo = fields[7];

    //Validações

    if (strlen(iata) != 3 || !isalpha((unsigned char)iata[0]) || 
        !isalpha((unsigned char)iata[1]) || !isalpha((unsigned char)iata[2])) {
        r.motivo = "Codigo IATA invalido";
        return r;
    }
    if(strlen(nome)||strlen(cidade)||strlen(pais)){
        r.motivo="existem campos vazio";
        return r;
    }
    if(!verifica_coordenadas||!validar_latitude||!validar_longitude){
        r.motivo="coodenadas inválidas";
        return r;
    }
    
    Aeroporto *a = malloc(sizeof(Aeroporto));
    if (!a) {
        printf("erro ao alocar memoria\n");
    }

    
    a->iata = strdup(iata);for (char *p = a->iata; *p; ++p) *p = toupper(*p);
    a->nome = strdup(nome);
    a->cidade = strdup(cidade);
    a->pais = strdup(pais);
    a->latitude = latitude;
    a->longitude = longitude;
    a->tipo = strdup(tipo);

    r.valido = true;
    r.airport = a;
    r.motivo = NULL;
    return r;
}
