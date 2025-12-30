#ifndef PARSER_PASSAGEIRO_H
#define PARSER_PASSAGEIRO_H

#include "csv_parser.h"
#include "passageiros.h"

/**
 * @file parser_passageiro.h
 * @brief Validação e parsing de linhas do CSV de passageiros.
 */

/**
 * @brief Faz o parsing de uma linha do CSV de passageiros (campos já divididos).
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos em `fields`.
 * @return Resultado de parsing com indicação de validade e/ou entidade.
 */
ParseResultado* parse_passageiro_line(char **fields, int n_fields);

/**
 * @brief Cria um passageiro a partir dos campos do CSV.
 *
 * Função com assinatura compatível com `csv_parse_file()`.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos.
 * @param ctx Contexto opcional (pode ser `NULL`).
 * @return Ponteiro para `Passageiro` criado, ou `NULL` se inválido.
 */
void* criar_passageiro(char **fields, int n_fields, void *ctx);

/**
 * @brief Faz parsing do ficheiro CSV de passageiros.
 *
 * @param caminho_csv Caminho para o CSV de passageiros.
 * @param ficheiro_erros Caminho para o ficheiro onde escrever linhas inválidas.
 * @param gestor Gestor onde inserir as entidades.
 * @param separador Separador CSV.
 * @return Número de passageiros válidos inseridos.
 */
int csv_parser_passageiros(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
);

#endif