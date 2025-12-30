#ifndef PARSER_AEROPORTO_H
#define PARSER_AEROPORTO_H

#include "csv_parser.h"
#include "aeroportos.h"

/**
 * @file parser_aeroporto.h
 * @brief Validação e parsing de linhas do CSV de aeroportos.
 */

/**
 * @brief Verifica o formato de uma coordenada (latitude/longitude).
 *
 * Valida a representação textual de uma coordenada e limita o número de
 * dígitos conforme `digitos_max`.
 *
 * @param s String a validar.
 * @param digitos_max Número máximo de dígitos permitido.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int verifica_coordenadas(const char *s, int digitos_max);

/**
 * @brief Valida uma latitude (string).
 *
 * @param s Latitude a validar.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int validar_latitude(const char *s);

/**
 * @brief Valida uma longitude (string).
 *
 * @param s Longitude a validar.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int validar_longitude(const char *s);

/**
 * @brief Verifica se o tipo de aeroporto é válido.
 *
 * @param tipo String com o tipo.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int tipo_aeroporto_valido(char *tipo);

/**
 * @brief Valida um código IATA.
 *
 * @param s Código IATA a validar.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int validar_iata(const char *s);

/**
 * @brief Valida um código ICAO.
 *
 * @param s Código ICAO a validar.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int validar_icao(const char *s);

/**
 * @brief Faz o parsing de uma linha do CSV de aeroportos (campos já divididos).
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos em `fields`.
 * @return Resultado de parsing com indicação de validade e/ou entidade.
 */
ParseResultado* parse_airport_line(char **fields, int n_fields);

/**
 * @brief Cria um aeroporto a partir dos campos do CSV.
 *
 * Função com assinatura compatível com `csv_parse_file()`.
 *
 * @param fields Vetor de strings com os campos.
 * @param n_fields Número de campos.
 * @param ctx Contexto opcional (pode ser `NULL`).
 * @return Ponteiro para `Aeroporto` criado, ou `NULL` se inválido.
 */
void* criar_aeroporto(char **fields, int n_fields, void *ctx);

/**
 * @brief Faz parsing do ficheiro CSV de aeroportos.
 *
 * Função de conveniência que abre o CSV e delega para o parser genérico.
 *
 * @param caminho_csv Caminho para o CSV de aeroportos.
 * @param ficheiro_erros Caminho para o ficheiro onde escrever linhas inválidas.
 * @param gestor Gestor onde inserir as entidades.
 * @param separador Separador CSV.
 * @return Número de aeroportos válidos inseridos.
 */
int csv_parser_airports(
    const char *caminho_csv,
    const char *ficheiro_erros,
    void *gestor,
    char separador
);

#endif