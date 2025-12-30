#ifndef GESTORGLOBAL_H
#define GESTORGLOBAL_H

#include "gestor_aeroporto.h"
#include "gestor_voo.h"
#include "gestor_aeronave.h"
#include "gestor_passageiro.h"
#include "gestor_reserva.h"

// Opaque forward declaration to hide internals from users of this API
typedef struct SistemaGestor SistemaGestor;

/**
 * @file gestorglobal.h
 * @brief API do gestor global (agregador de subgestores).
 *
 * O gestor global agrega os vários gestores (aeroportos, voos, aeronaves,
 * passageiros e reservas) e fornece acesso controlado a cada um deles.
 */

/**
 * @brief Destrói o gestor global e liberta os recursos associados.
 *
 * Esta função deve ser chamada no final da execução para libertar toda a
 * memória associada ao sistema (incluindo subgestores).
 *
 * @param gestorGlobal Instância do gestor global a destruir.
 * @return Nada (função `void`).
 */
void destruir_gestor_global(SistemaGestor *gestorGlobal);

/**
 * @brief Cria e inicializa um gestor global.
 *
 * Aloca o gestor e inicializa todos os subgestores internos.
 *
 * @return Ponteiro para o gestor global criado, ou `NULL` em caso de falha.
 */
SistemaGestor *criar_gestor_global(void);

// Acesso controlado aos subgestores
/**
 * @brief Obtém o gestor de aeroportos.
 *
 * @param gestor Gestor global.
 * @return Ponteiro para o subgestor de aeroportos.
 */
AeroportoPtrManager *gestor_global_get_aeroportos(SistemaGestor *gestor);

/**
 * @brief Obtém o gestor de voos.
 *
 * @param gestor Gestor global.
 * @return Ponteiro para o subgestor de voos.
 */
VooPtrManager *gestor_global_get_voos(SistemaGestor *gestor);

/**
 * @brief Obtém o gestor de aeronaves.
 *
 * @param gestor Gestor global.
 * @return Ponteiro para o subgestor de aeronaves.
 */
AeronavePtrManager *gestor_global_get_aeronaves(SistemaGestor *gestor);

/**
 * @brief Obtém o gestor de passageiros.
 *
 * @param gestor Gestor global.
 * @return Ponteiro para o subgestor de passageiros.
 */
PassageiroPtrManager *gestor_global_get_passageiros(SistemaGestor *gestor);

/**
 * @brief Obtém o gestor de reservas.
 *
 * @param gestor Gestor global.
 * @return Ponteiro para o subgestor de reservas.
 */
ReservaPtrManager *gestor_global_get_reservas(SistemaGestor *gestor);

#endif // GESTORGLOBAL_H