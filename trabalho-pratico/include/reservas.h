#ifndef RESERVAS
#define RESERVAS

#include <stddef.h>
#include <stdbool.h>
#include <glib.h>

// Estrutura opaca - definição completa no .c
typedef struct reserva Reserva;
typedef struct ReservaManager ReservaManager;

// Gestor baseado em ponteiros (usa GArray)
typedef struct {
    GArray *reservas; // array de (Reserva*)
} ReservaPtrManager;

// Criação e destruição do gestor
ReservaManager* reserva_manager_create();
void reserva_manager_destroy(ReservaManager *manager);

// Adicionar reserva (cópia profunda)
int reserva_manager_add(ReservaManager *manager, Reserva reserva);

// Pesquisa
Reserva* reserva_manager_find_by_id(ReservaManager *manager, const char *reservation_id);
Reserva* reserva_manager_find_by_document(ReservaManager *manager, const char *document_number);

// Obter número de reservas
int reserva_manager_get_count(ReservaManager *manager);

// Obter reserva por índice (para iteração)
Reserva* reserva_manager_get_at_index(ReservaManager *manager, int index);

// Resumo formatado de uma reserva
int reserva_summary(const Reserva *reserva, char *buf, size_t bufsize);

/* Pointer-based manager API */
ReservaPtrManager* reserva_ptr_manager_create();
void reserva_ptr_manager_destroy(ReservaPtrManager *manager);
int reserva_ptr_manager_add_from_ptr(ReservaPtrManager *manager, Reserva *entidade);
Reserva* reserva_ptr_manager_find_by_id(ReservaPtrManager *manager, const char *reservation_id);

// Adapter compatível com csv_parser genérico
int reserva_ptr_manager_add_from_ptr_adapter(void *gestor, const void *entidade);

// Getters para Reserva
const char* reserva_get_id(const Reserva *r);
const char* reserva_get_flight_ids(const Reserva *r);
const char* reserva_get_document_number(const Reserva *r);
const char* reserva_get_seat(const Reserva *r);
double reserva_get_price(const Reserva *r);
bool reserva_get_extra_luggage(const Reserva *r);
bool reserva_get_priority_boarding(const Reserva *r);
const char* reserva_get_qr_code(const Reserva *r);

#endif