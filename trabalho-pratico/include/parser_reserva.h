#ifndef PARSER_RESERVA_H
#define PARSER_RESERVA_H

#include "csv_parser.h"
#include "reservas.h"
#include "gestorglobal.h"

/**
 * @file parser_reserva.h
 * @brief Validação e parsing de linhas do CSV de reservas.
 */

/**
 * @brief Faz o parsing de uma linha do CSV de reservas (campos já divididos).
 *
 * Pode usar o `gestor` para validações cruzadas.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos em `fields`.
 * @param gestor Gestor global com dados carregados.
 * @return Resultado de parsing com indicação de validade e/ou entidade.
 */
ParseResultado* parse_reserva_line(char **fields, int n_fields, SistemaGestor *gestor);

/**
 * @brief Cria uma reserva a partir dos campos do CSV.
 *
 * Função com assinatura compatível com `csv_parse_file()`.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos.
 * @param ctx Contexto opcional (normalmente um `SistemaGestor*`).
 * @return Ponteiro para `Reserva` criado, ou `NULL` se inválido.
 */
void* criar_reserva(char **fields, int n_fields, void *ctx);

/**
 * @brief Faz parsing do ficheiro CSV de reservas.
 *
 * @param caminho_csv Caminho para o CSV de reservas.
 * @param ficheiro_erros Caminho para o ficheiro onde escrever linhas inválidas.
 * @param gestor Gestor onde inserir as entidades.
 * @param gestorGlob Gestor global (para validação cruzada).
 * @param separador Separador CSV.
 * @return Número de reservas válidas inseridas.
 */
int csv_parser_reservas(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    SistemaGestor *gestorGlob,
    char separador
);

#endif