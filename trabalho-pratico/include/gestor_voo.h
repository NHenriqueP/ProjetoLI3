#ifndef GESTOR_VOO_H
#define GESTOR_VOO_H

#include <glib.h>
#include "voos.h"

typedef struct voo_ptr_manager VooPtrManager;

/**
 * @file gestor_voo.h
 * @brief Gestor de voos (armazenamento, pesquisa e estatísticas).
 */

/**
 * @brief Estrutura de estatísticas de atraso por companhia.
 *
 * Usada em tabelas agregadas onde a chave é a `airline`.
 */
typedef struct {
	char *airline;
	long delayed_flights_count;
	long long total_delay_minutes;
} AirlineDelayStats;

/**
 * @brief Cria um gestor de voos.
 *
 * @return Ponteiro para o gestor criado, ou `NULL` em caso de falha.
 */
VooPtrManager* voo_ptr_manager_create();

/**
 * @brief Destrói um gestor de voos.
 *
 * @param manager Gestor a destruir.
 * @return Nada (função `void`).
 */
void voo_ptr_manager_destroy(VooPtrManager *manager);

/**
 * @brief Adapter de inserção usado pelo parser genérico.
 *
 * @param gestor Ponteiro genérico para `VooPtrManager`.
 * @param entidade Ponteiro para a entidade a inserir (tipicamente `Voo*`).
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int voo_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

/**
 * @brief Procura um voo pelo seu identificador.
 *
 * @param manager Gestor de voos.
 * @param id Identificador do voo.
 * @return Ponteiro para o voo, ou `NULL` se não existir.
 */
Voo* voo_ptr_manager_find_by_id(VooPtrManager *manager, const char *id);

/**
 * @brief Obtém a árvore de voos agregada por data.
 *
 * @param manager Gestor de voos.
 * @return Ponteiro para um `GTree` com a agregação por data (propriedade do gestor).
 */
GTree * manager_get_voos_por_data(VooPtrManager *manager);

/**
 * @brief Número total de voos carregados.
 *
 * @param manager Gestor de voos.
 * @return Número de voos.
 */
guint voo_ptr_manager_count(const VooPtrManager *manager);

/**
 * @brief Acesso indexado a um voo.
 *
 * @param manager Gestor de voos.
 * @param index Índice (0..count-1).
 * @return Ponteiro para o voo no índice, ou `NULL` se o índice for inválido.
 */
const Voo* voo_ptr_manager_get(const VooPtrManager *manager, guint index);

// Tabela com estatísticas agregadas de atrasos por companhia (apenas voos DELAYED).
// Chave: (const char*) airline
// Valor: (AirlineDelayStats*)
// Nota: retorno é propriedade do manager; tratar como read-only.
/**
 * @brief Obtém estatísticas agregadas de atrasos por companhia.
 *
 * A tabela devolvida é propriedade do gestor e deve ser tratada como read-only.
 *
 * @param manager Gestor de voos.
 * @return `GHashTable*` com chave `(const char*)` e valor `(AirlineDelayStats*)`.
 */
GHashTable *voo_ptr_manager_get_delay_stats_by_airline(VooPtrManager *manager);

// Contagem de voos por aeronave (exclui CANCELLED), agregada no carregamento.
/**
 * @brief Obtém a contagem de voos associados a uma aeronave.
 *
 * Tipicamente exclui voos com estado CANCELLED, conforme a lógica de carregamento.
 *
 * @param manager Gestor de voos.
 * @param aircraft_id Identificador da aeronave.
 * @return Contagem de voos para essa aeronave.
 */
long voo_ptr_manager_get_flight_count_by_aircraft(VooPtrManager *manager, const char *aircraft_id);

#endif