#include "parser_passageiro.h"

#include <ctype.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <csv_parser.h>

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

ParseResultado parse_passageiro_line(char **fields, int n_fields) {
    ParseResultado r;
    r.valido = false;
    r.entidade = NULL;


    if (n_fields < 10) {
        return r;
    }

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

    size_t document_number_len = 0;
    size_t first_name_len = 0;
    size_t last_name_len = 0;
    size_t dob_len = 0;
    size_t nationality_len = 0;
    size_t gender_len = 0;
    size_t email_len = 0;
    size_t phone_len = 0;
    size_t address_len = 0;

    if (!document_number) {
        return r;
    }

    document_number_len = strlen(document_number);
    if (!validar_document_number(document_number, document_number_len)) {
        return r;
    }

    if (!first_name) {
        return r;
    }

    first_name_len = strlen(first_name);
    if (first_name_len == 0) {
        return r;
    }

    if (!last_name) {
        return r;
    }

    last_name_len = strlen(last_name);
    if (last_name_len == 0) {
        return r;
    }

    if (!dob) {
        return r;
    }

    dob_len = strlen(dob);
    if (!validar_data(dob)) {
        return r;
    }

    if (!nationality) {
        return r;
    }

    nationality_len = strlen(nationality);
    if (nationality_len == 0) {
        return r;
    }

    if (!gender) {
        return r;
    }

    gender_len = strlen(gender);
    if (!validar_gender(gender)) {
        return r;
    }

    if (!email) {
        return r;
    }

    email_len = strlen(email);
    if (!validar_email(email)) {
        return r;
    }

    if (!phone) {
        return r;
    }

    phone_len = strlen(phone);
    if (phone_len == 0) {
        return r;
    }

    if (!address) {
        return r;
    }

    address_len = strlen(address);
    if (address_len == 0) {
        return r;
    }

    Passageiro *p = g_new0(Passageiro, 1);
    if (!p) {
        return r;
    }

    p->document_number = (char*)g_memdup2(document_number, document_number_len + 1);
    p->first_name = (char*)g_memdup2(first_name, first_name_len + 1);
    p->last_name = (char*)g_memdup2(last_name, last_name_len + 1);
    p->dob = (char*)g_memdup2(dob, dob_len + 1);
    p->nationality = (char*)g_memdup2(nationality, nationality_len + 1);
    p->gender = (char*)g_memdup2(gender, gender_len + 1);
    p->email = (char*)g_memdup2(email, email_len + 1);
    p->phone = (char*)g_memdup2(phone, phone_len + 1);
    p->address = (char*)g_memdup2(address, address_len + 1);

    size_t photo_len = photo ? strlen(photo) : 0;
    p->photo = (char*)g_memdup2(photo ? photo : "", photo_len + 1);

    if (!p->document_number || !p->first_name || !p->last_name || !p->dob ||
        !p->nationality || !p->gender || !p->email || !p->phone || !p->address || !p->photo) {
        g_free(p->document_number);
        g_free(p->first_name);
        g_free(p->last_name);
        g_free(p->dob);
        g_free(p->nationality);
        g_free(p->gender);
        g_free(p->email);
        g_free(p->phone);
        g_free(p->address);
        g_free(p->photo);
        g_free(p);
        return r;
    }

    r.valido = true;
    r.entidade = p;
    return r;
}

int csv_parser_passageiros(
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
        ParseResultado resultado = parse_passageiro_line(fields, n_fields);

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
            passageiro_ptr_manager_add_from_ptr_adapter(gestor, resultado.entidade);
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