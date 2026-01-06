#ifndef AEROPORTO
#define AEROPORTO
#include <stddef.h> // size_t

typedef struct Aeroporto Aeroporto;

/**
 * @file aeroportos.h
 * @brief API pública da entidade Aeroporto.
 *
 * Define um tipo opaco `Aeroporto` e funções para criar, destruir e aceder
 * a informação de aeroportos.
 */

/**
 * @brief Cria um aeroporto.
 *
 * Copia internamente os campos fornecidos para a estrutura `Aeroporto`.
 *
 * @param iata Código IATA do aeroporto.
 * @param nome Nome do aeroporto.
 * @param cidade Cidade.
 * @param pais País.
 * @param tipo Tipo de aeroporto.
 * @return Ponteiro para o aeroporto criado, ou `NULL` em caso de falha.
 */
Aeroporto* aeroporto_create(const char *iata,
                         	   const char *nome,
                            const char *cidade,
                            const char *pais,
                            const char *tipo);

/**
 * @brief Destrói um aeroporto.
 *
 * Liberta toda a memória associada à estrutura.
 *
 * @param aeroporto Aeroporto a destruir.
 * @return Nada (função `void`).
 */
void aeroporto_destroy(Aeroporto *aeroporto);

/**
 * @brief Obtém o código IATA de um aeroporto.
 *
 * @param aeroporto Aeroporto.
 * @return String com o IATA (propriedade do aeroporto; não libertar).
 */
const char* aeroporto_get_iata(const Aeroporto *aeroporto);

// Resumo formatado de um aeroporto no buffer fornecido, usando um separador específico
// Produz: IATA<sep>Nome<sep>Cidade<sep>País<sep>Tipo
/**
 * @brief Escreve um resumo do aeroporto com separador configurável.
 *
 * Produz: IATA<sep>Nome<sep>Cidade<sep>País<sep>Tipo
 *
 * @param airport Aeroporto.
 * @param buf Buffer de saída.
 * @param bufsize Tamanho do buffer.
 * @param sep Separador a usar.
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int airport_summary_delim(const Aeroporto *airport, char *buf, size_t bufsize, char sep);

#endif 