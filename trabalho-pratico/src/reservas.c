#include "reservas.h"
#include <stdio.h>

// ==================== GETTERS ====================

const char* reserva_get_id(const Reserva *r) {
    if (r) return r->reservation_id;
    return NULL;
}

const char* reserva_get_flight_ids(const Reserva *r) {
    if (r) return r->flight_ids;
    return NULL;
}

const char* reserva_get_document_number(const Reserva *r) {
    if (r) return r->document_number;
    return NULL;
}

const char* reserva_get_seat(const Reserva *r) {
    if (r) return r->seat;
    return NULL;
}

double reserva_get_price(const Reserva *r) {
    if (r) return r->price;
    return -1.0;
}

bool reserva_get_extra_luggage(const Reserva *r) {
    if (r) return r->extra_luggage;
    return false;
}

bool reserva_get_priority_boarding(const Reserva *r) {
    if (r) return r->priority_boarding;
    return false;
}

const char* reserva_get_qr_code(const Reserva *r) {
    if (r) return r->qr_code;
    return NULL;
}

// Produz um resumo CSV da reserva
int reserva_summary(const Reserva *reserva, char *buf, size_t bufsize) {
    if (!reserva || !buf || bufsize == 0) return 0;
    
    int n = snprintf(buf, bufsize, "%s,%s,%s,%s,%.2f,%s,%s",
                     reserva->reservation_id ? reserva->reservation_id : "",
                     reserva->flight_ids ? reserva->flight_ids : "",
                     reserva->document_number ? reserva->document_number : "",
                     reserva->seat ? reserva->seat : "",
                     reserva->price,
                     reserva->extra_luggage ? "true" : "false",
                     reserva->priority_boarding ? "true" : "false");
    
    if (n < 0) return 0;
    if ((size_t)n >= bufsize) return 0; // truncado
    return n;
}

// Parser movido para parser_reserva.c