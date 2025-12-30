#ifndef PARSER_VOO_H
#define PARSER_VOO_H

#include "csv_parser.h"
#include "voos.h"
#include "gestorglobal.h"

/**
 * @file parser_voo.h
 * @brief Validação e parsing de linhas do CSV de voos.
 */

/**
 * @brief Faz o parsing de uma linha do CSV de voos (campos já divididos).
 *
 * Pode usar o `gestor` (gestor global) para validações cruzadas (ex.: verificar
 * existência de aeroportos/aeronaves referenciados).
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos em `fields`.
 * @param gestor Gestor global com dados já carregados (para validação cruzada).
 * @return Resultado de parsing com indicação de validade e/ou entidade.
 */
ParseResultado* parse_voo_line(char **fields, int n_fields, SistemaGestor *gestor);

/**
 * @brief Cria um voo a partir dos campos do CSV.
 *
 * Função com assinatura compatível com `csv_parse_file()`.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos.
 * @param ctx Contexto opcional (normalmente um `SistemaGestor*`).
 * @return Ponteiro para `Voo` criado, ou `NULL` se inválido.
 */
void* criar_voo(char **fields, int n_fields, void *ctx);

/**
 * @brief Faz parsing do ficheiro CSV de voos.
 *
 * @param caminho_csv Caminho para o CSV de voos.
 * @param ficheiro_erros Caminho para o ficheiro onde escrever linhas inválidas.
 * @param gestor Gestor onde inserir as entidades.
 * @param sistema_gestor Gestor global (para validação cruzada).
 * @param separador Separador CSV.
 * @return Número de voos válidos inseridos.
 */
int csv_parser_voos(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    SistemaGestor *sistema_gestor,
    char separador
);

#endif