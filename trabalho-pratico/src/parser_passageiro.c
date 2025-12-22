#include "parser_passageiro.h"
#include <ctype.h>
#include <errno.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include "csv_parser.h"
#include "gestor_passageiro.h"

static int validar_document_number(const char *s, size_t len) {
    if (!s || len != 9) return 0;
    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return s[9] == '\0';
}

static int validar_data(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    if (s[4] != '-' || s[7] != '-') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    int year = (s[0]-'0')*1000 + (s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0');
    int month = (s[5]-'0')*10 + (s[6]-'0');
    int day = (s[8]-'0')*10 + (s[9]-'0');
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    return 1;
}

static int validar_gender(const char *s) {
    if (!s || strlen(s) != 1) return 0;
    return (s[0] == 'M' || s[0] == 'F' || s[0] == 'O');
}

static int validar_email(const char *s) {
    if (!s || strlen(s) == 0) return 0;
    int tem_arroba = 0;
    int tem_ponto_depois_arroba = 0;
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '@') {
            tem_arroba = 1;
        }
        if (tem_arroba && s[i] == '.') {
            tem_ponto_depois_arroba = 1;
        }
    }
    return tem_arroba && tem_ponto_depois_arroba;
}

void* criar_passageiro(char **fields, int n_fields, void *ctx) {
    (void)ctx;
    if (n_fields < 10) return NULL;

    char *document_number = fields[0];
    char *first_name      = fields[1];
    char *last_name       = fields[2];
    char *dob             = fields[3];
    char *nationality     = fields[4];
    char *gender          = fields[5];
    char *email           = fields[6];
    char *phone           = fields[7];
    char *address         = fields[8];
    char *photo           = fields[9];

    if (!document_number) return NULL;

    size_t document_number_len = strlen(document_number);
    if (!validar_document_number(document_number, document_number_len)) return NULL;

    if (!first_name) return NULL;
    size_t first_name_len = strlen(first_name);
    if (first_name_len == 0) return NULL;

    if (!last_name) return NULL;
    size_t last_name_len = strlen(last_name);
    if (last_name_len == 0) return NULL;

    if (!dob) return NULL;
    if (!validar_data(dob)) return NULL;

    if (!nationality) return NULL;
    size_t nationality_len = strlen(nationality);
    if (nationality_len == 0) return NULL;

    if (!gender) return NULL;
    size_t gender_len = strlen(gender);
    if (!validar_gender(gender) || gender_len == 0) return NULL;

    if (!email) return NULL;
    size_t email_len = strlen(email);
    if (!validar_email(email) || email_len == 0) return NULL;

    if (!phone) return NULL;
    size_t phone_len = strlen(phone);
    if (phone_len == 0) return NULL;

    if (!address) return NULL;
    size_t address_len = strlen(address);
    if (address_len == 0) return NULL;

    return passageiro_create(
        document_number,
        first_name,
        last_name,
        dob,
        nationality
    );
}

ParseResultado* parse_passageiro_line(char **fields, int n_fields) {
    ParseResultado *r = parse_resultado_create();
    if (!r) return NULL;

    Passageiro *p = (Passageiro*)criar_passageiro(fields, n_fields, NULL);
    if (!p) return r;

    parse_resultado_mark_valid(r, p);
    return r;
}

int csv_parser_passageiros(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
) {
    FILE *ficheiro = fopen(caminho_csv, "r");
    if (!ficheiro) {
        fputs("Erro ao abrir ficheiro '", stderr);
        fputs(caminho_csv ? caminho_csv : "<NULL>", stderr);
        fputs("': ", stderr);
        fputs(strerror(errno), stderr);
        fputc('\n', stderr);
        return 0;
    }

    FILE *ficheiro_erros_ptr = fopen(ficheiro_erros, "w");
    if (!ficheiro_erros_ptr) {
        fputs("Erro ao criar ficheiro de erros: ", stderr);
        fputs(ficheiro_erros ? ficheiro_erros : "<NULL>", stderr);
        fputc('\n', stderr);
        fclose(ficheiro);
        return 0;
    }

    int contador_validos = csv_generic_parser(
        ficheiro,
        ficheiro_erros_ptr,
        gestor,
        NULL,
        NULL,
        criar_passageiro,
        passageiro_ptr_manager_add_from_ptr_adapter,
        separador
    );

    fclose(ficheiro_erros_ptr);
    fclose(ficheiro);

    return contador_validos;
}