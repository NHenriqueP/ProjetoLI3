#ifndef VOO
#define VOO

#include <stddef.h>
#include <glib.h>

typedef struct Date Date;

typedef struct Hora  Hora;

typedef struct Horario Horario;

/**
 * @file voos.h
 * @brief API pública da entidade Voo e tipos auxiliares de data/horário.
 */

/**
 * @brief Estado de um voo.
 */
typedef enum {
    ON_TIME,
    DELAYED,
    CANCELLED
} Status;

typedef struct Voo Voo;

/**
 * @brief Destrói um voo.
 *
 * Liberta a memória associada à estrutura `Voo`.
 *
 * @param voo Voo a destruir.
 * @return Nada (função `void`).
 */
void voo_destroy(Voo *voo);

/**
 * @brief Cria um voo.
 *
 * Constrói um objeto `Voo` com base nos parâmetros fornecidos.
 *
 * @param idvoo Identificador do voo.
 * @param departure Horário estimado de partida.
 * @param actualdeparture Horário real de partida.
 * @param estado Estado do voo.
 * @param iataorigem IATA de origem.
 * @param iatadestino IATA de destino.
 * @param aircraft Identificador da aeronave.
 * @param airline Companhia aérea.
 * @return Ponteiro para o voo criado, ou `NULL` em caso de falha.
 */
Voo* voo_create(const char *idvoo,
                const Horario *departure,
                const Horario *actualdeparture,
                Status estado,
                const char *iataorigem,
                const char *iatadestino,
                const char *aircraft,
                const char *airline);

/**
 * @brief Liberta um objeto `Horario`.
 *
 * @param h Horário a libertar.
 * @return Nada (função `void`).
 */
void horario_free(Horario *h);

/**
 * @brief Copia um horário.
 *
 * @param dest Destino.
 * @param src Fonte.
 * @return Nada (função `void`).
 */
void horario_copy(Horario *dest, const Horario *src);

/**
 * @brief Reinicializa um horário.
 *
 * @param h Horário a resetar.
 * @return Nada (função `void`).
 */
void horario_reset(Horario *h);

/**
 * @brief Cria um objeto `Date`.
 *
 * @return Ponteiro para `Date` criado.
 */
Date *date_new(void);

/**
 * @brief Obtém a data associada a um `Horario`.
 *
 * @param h Horário.
 * @return Ponteiro para `Date` (propriedade do `Horario`; não libertar).
 */
const Date *horario_get_date(const Horario *h);

/**
 * @brief Cria um objeto `Horario`.
 *
 * @return Ponteiro para `Horario` criado.
 */
Horario *horario_new(void);

/**
 * @brief Define uma data (ano/mês/dia).
 *
 * @param d Data a preencher.
 * @param year Ano.
 * @param month Mês.
 * @param day Dia.
 * @return Nada (função `void`).
 */
void set_date(Date *d, int year, int month, int day);

/**
 * @brief Copia uma data de `src` para `dest`.
 *
 * @param dest Destino.
 * @param src Fonte.
 * @return Nada (função `void`).
 */
void passar_date (Date *dest, const Date *src);

/**
 * @brief Compara datas pelo dia (compatível com GLib).
 *
 * Função com assinatura apropriada para callbacks de comparação (ex.: `GTree`).
 *
 * @param a Primeiro elemento.
 * @param b Segundo elemento.
 * @return Valor <0, 0 ou >0 conforme ordenação.
 */
gint comparar_dates_dia(gconstpointer a, gconstpointer b);

/**
 * @brief Define um horário completo (data e hora).
 *
 * @param h Horário a preencher.
 * @param year Ano.
 * @param month Mês.
 * @param day Dia.
 * @param hora Hora.
 * @param minuto Minuto.
 * @return Nada (função `void`).
 */
void set_horario(Horario *h, int year, int month, int day, int hora, int minuto);

/**
 * @brief Compara dois horários.
 *
 * @param a Primeiro horário.
 * @param b Segundo horário.
 * @return Valor <0, 0 ou >0 conforme ordenação.
 */
int comparar_horarios(const Horario *a, const Horario *b);

/**
 * @brief Compara duas datas.
 *
 * @param a Primeira data.
 * @param b Segunda data.
 * @return Valor <0, 0 ou >0 conforme ordenação.
 */
int comparar_dates(const Date *a, const Date *b);

/**
 * @brief Obtém o identificador do voo.
 *
 * @param voo Voo.
 * @return String com o id (propriedade do voo; não libertar).
 */
const char* voo_get_id(const Voo *voo);

/**
 * @brief Obtém o estado do voo.
 *
 * @param voo Voo.
 * @return Estado do voo.
 */
Status voo_get_status(const Voo *voo);

/**
 * @brief Obtém o identificador da aeronave associada ao voo.
 *
 * @param voo Voo.
 * @return String com o id da aeronave (propriedade do voo; não libertar).
 */
const char* voo_get_aircraft(const Voo *voo);

/**
 * @brief Obtém o IATA de origem.
 *
 * @param voo Voo.
 * @return String com o IATA de origem (propriedade do voo; não libertar).
 */
const char* voo_get_iata_origem(const Voo *voo);

/**
 * @brief Obtém o IATA de destino.
 *
 * @param voo Voo.
 * @return String com o IATA de destino (propriedade do voo; não libertar).
 */
const char* voo_get_iata_destino(const Voo *voo);

/**
 * @brief Obtém o horário estimado de partida.
 *
 * @param voo Voo.
 * @return Ponteiro para `Horario` (propriedade do voo; não libertar).
 */
const Horario* voo_get_departure(const Voo *voo);

/**
 * @brief Obtém o horário real de partida.
 *
 * @param voo Voo.
 * @return Ponteiro para `Horario` (propriedade do voo; não libertar).
 */
const Horario* voo_get_actual_departure(const Voo *voo);

/**
 * @brief Obtém o horário estimado de chegada.
 *
 * @param voo Voo.
 * @return Ponteiro para `Horario` (propriedade do voo; não libertar).
 */
const Horario* voo_get_arrival(const Voo *voo);

/**
 * @brief Obtém o horário real de chegada.
 *
 * @param voo Voo.
 * @return Ponteiro para `Horario` (propriedade do voo; não libertar).
 */
const Horario* voo_get_actual_arrival(const Voo *voo);

/**
 * @brief Obtém o gate associado ao voo.
 *
 * @param voo Voo.
 * @return Gate.
 */
int voo_get_gate(const Voo *voo);

/**
 * @brief Obtém a companhia aérea do voo.
 *
 * @param voo Voo.
 * @return String com a companhia (propriedade do voo; não libertar).
 */
const char* voo_get_airline(const Voo *voo);

/**
 * @brief Obtém o URL de tracking do voo.
 *
 * @param voo Voo.
 * @return String com o URL (propriedade do voo; não libertar).
 */
const char* voo_get_tracking_url(const Voo *voo);

/**
 * @brief Valida o formato de um identificador de voo.
 *
 * @param s String com o id.
 * @return Valor inteiro indicando validade (0/1 conforme implementação).
 */
int validar_flight_id(const char *s);

/**
 * @brief Escreve um resumo do voo para um buffer.
 *
 * @param voo Voo.
 * @param buf Buffer de saída.
 * @param bufsize Tamanho do buffer.
 * @return Inteiro de estado (por convenção, 0/1 conforme implementação).
 */
int voo_summary(const Voo *voo, char *buf, size_t bufsize);

// Diferença, em minutos, entre a hora real e estimada de partida (actualdeparture - departure).
// Útil para queries que calculam atraso sem expor a estrutura interna de Horario.
/**
 * @brief Calcula o atraso de partida em minutos.
 *
 * Retorna a diferença (actualdeparture - departure), em minutos.
 *
 * @param voo Voo.
 * @return Atraso em minutos.
 */
long long voo_get_departure_delay_minutes(const Voo *voo);

// Número de dias desde 1970-01-01 para a data de partida estimada (departure).
/**
 * @brief Converte a data de partida estimada para um número de dias.
 *
 * Útil para agregações por dia, sem expor detalhes internos do tipo `Date`.
 *
 * @param voo Voo.
 * @return Número de dias desde 1970-01-01.
 */
long long voo_get_departure_day_number(const Voo *voo);

#endif