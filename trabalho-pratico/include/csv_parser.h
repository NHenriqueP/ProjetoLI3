#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct ParseResultado ParseResultado;

/**
 * @file csv_parser.h
 * @brief Funções auxiliares para parsing de ficheiros CSV.
 *
 * Este módulo fornece uma função genérica de parsing linha-a-linha e
 * utilitários para dividir uma linha CSV com suporte a aspas.
 */

/**
 * @brief Cria um objeto de resultado de parsing.
 *
 * Inicializa uma estrutura `ParseResultado` marcada como inválida e sem
 * entidade associada.
 *
 * @return Ponteiro para o resultado alocado, ou `NULL` em caso de falha.
 */
ParseResultado* parse_resultado_create(void);

/**
 * @brief Marca um resultado de parsing como válido.
 *
 * Associa ao resultado um ponteiro genérico para a entidade criada pelo
 * parser específico.
 *
 * @param resultado Resultado a atualizar.
 * @param entidade Ponteiro para a entidade associada ao resultado.
 * @return Nada (função `void`).
 */
void parse_resultado_mark_valid(ParseResultado *resultado, void *entidade);

/**
 * @brief Indica se um resultado de parsing é válido.
 *
 * @param resultado Resultado a consultar.
 * @return `true` se válido; `false` caso contrário (ou se `resultado` for `NULL`).
 */
bool parse_resultado_is_valid(const ParseResultado *resultado);

/**
 * @brief Obtém a entidade associada a um resultado de parsing.
 *
 * @param resultado Resultado de parsing.
 * @return Ponteiro para a entidade associada, ou `NULL` se não existir.
 */
void* parse_resultado_get_entidade(const ParseResultado *resultado);

/**
 * @brief Destrói um resultado de parsing.
 *
 * Liberta apenas a estrutura `ParseResultado`. A entidade associada (se
 * existir) deve ser gerida pelo chamador/gestor, conforme a política do
 * parser específico.
 *
 * @param resultado Resultado a destruir.
 * @return Nada (função `void`).
 */
void parse_resultado_destroy(ParseResultado *resultado);

/**
 * @brief Divide uma linha CSV em campos, suportando aspas.
 *
 * Esta função faz tokenização in-place: substitui separadores por "\\0"
 * e escreve em out ponteiros para o início de cada campo. Aspas duplas
 * consecutivas ("") são interpretadas como uma aspa literal.
 *
 * @param[in,out] s Linha a dividir (será modificada).
 * @param[in] sep Carácter separador (por exemplo ',').
 * @param[out] out Vetor onde são colocados os ponteiros para cada campo.
 * @param[in] max_out Capacidade máxima do vetor out (número de campos).
 * @return Número de campos encontrados (no máximo max_out).
 */
int csv_split_quoted(char *s, char sep, char **out, int max_out);

/**
 * @brief Parser genérico para um ficheiro CSV já aberto.
 *
 * Lê um CSV linha-a-linha, ignora o header e, para cada linha:
 * (1) divide em campos, (2) valida (se fornecido), (3) cria a entidade,
 * e (4) insere no gestor. Linhas inválidas podem ser escritas em
 * `error_file` (incluindo o header na primeira escrita).
 *
 * @param data_file Ficheiro de dados já aberto para leitura.
 * @param error_file Ficheiro de erros aberto para escrita (pode ser `NULL`).
 * @param gestor Estrutura do gestor onde inserir as entidades.
 * @param ctx Contexto opcional passado às callbacks.
 * @param is_valid_entidade Callback de validação (pode ser `NULL`).
 * @param create_entidade Callback de criação da entidade (obrigatório).
 * @param insert_entidade Callback de inserção no gestor (obrigatório).
 * @param separador Separador CSV a usar.
 * @return Número de entidades válidas inseridas.
 */
int csv_generic_parser(
    FILE *data_file,
    FILE *error_file,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
);

/**
 * @brief Abre e processa um ficheiro CSV por caminhos.
 *
 * Abre `data_path` para leitura e `error_path` para escrita, delegando o
 * processamento em `csv_generic_parser()`.
 *
 * @param data_path Caminho para o ficheiro CSV de dados.
 * @param error_path Caminho para o ficheiro CSV onde escrever linhas inválidas.
 * @param gestor Estrutura do gestor onde inserir as entidades.
 * @param ctx Contexto opcional passado às callbacks.
 * @param is_valid_entidade Callback de validação (pode ser `NULL`).
 * @param create_entidade Callback de criação da entidade (obrigatório).
 * @param insert_entidade Callback de inserção no gestor (obrigatório).
 * @param separador Separador CSV a usar.
 * @return Número de entidades válidas inseridas; valor negativo em caso de erro a abrir/criar ficheiros.
 */
int csv_parse_file(
    const char *data_path,
    const char *error_path,
    void *gestor,
    void *ctx,
    bool (*is_valid_entidade)(char **fields, int n_fields, void *ctx),
    void* (*create_entidade)(char **fields, int n_fields, void *ctx),
    int (*insert_entidade)(void *gestor, const void *entidade),
    char separador
);

#endif