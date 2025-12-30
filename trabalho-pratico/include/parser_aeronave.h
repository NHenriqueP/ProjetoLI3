#ifndef PARSER_AERONAVE_H
#define PARSER_AERONAVE_H

#include "csv_parser.h"
#include "aeronaves.h"



/**
 * @file parser_aeronave.h
 * @brief Validação e parsing de linhas do CSV de aeronaves.
 */

/**
 * @brief Faz o parsing de uma linha do CSV de aeronaves (campos já divididos).
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos em `fields`.
 * @return Resultado de parsing com indicação de validade e/ou entidade.
 */
ParseResultado* parse_aeronave_line(char **fields, int n_fields);

/**
 * @brief Cria uma aeronave a partir dos campos do CSV.
 *
 * Função com assinatura compatível com `csv_parse_file()`.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos.
 * @param ctx Contexto opcional (pode ser `NULL`).
 * @return Ponteiro para `Aeronave` criado, ou `NULL` se inválido.
 */
void* criar_aeronave(char **fields, int n_fields, void *ctx);

/**
 * @brief Faz parsing do ficheiro CSV de aeronaves.
 *
 * @param caminho_csv Caminho para o CSV de aeronaves.
 * @param ficheiro_erros Caminho para o ficheiro onde escrever linhas inválidas.
 * @param gestor Gestor onde inserir as entidades.
 * @param separador Separador CSV.
 * @return Número de aeronaves válidas inseridas.
 */
int csv_parser_aeronaves(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
);

#endif