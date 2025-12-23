#include "app_runner.h"
#include "gestor_aeronave.h"
#include "gestor_aeroporto.h"
#include "csv_parser.h"
#include "parser_aeroporto.h"
#include "parser_voo.h"
#include "parser_aeronave.h"
#include "parser_passageiro.h"
#include "parser_reserva.h"
#include "gestorglobal.h"
#include "gestor_passageiro.h"
#include "querie1.h"
#include "querie2.h"
#include "querie3.h"
#include "querie4.h"
#include "gestor_reserva.h"
#include "gestor_voo.h"
#include "querie5.h"

#include <stdio.h>
#include <string.h>

static int path_join2(char *dst, size_t dstsz, const char *dir, const char *file) {
    if (!dst || !dir || !file || dstsz == 0) return 0;
    dst[0] = '\0';
    size_t len = 0;
    size_t dirlen = strlen(dir);
    if (dirlen >= dstsz) return 0;
    for (size_t i = 0; i < dirlen; i++) dst[i] = dir[i];
    dst[dirlen] = '\0';
    len = dirlen;
    if (len + 1 < dstsz && dst[len - (len > 0)] != '/') {
        if (len + 1 >= dstsz) return 0;
        dst[len++] = '/';
        dst[len] = '\0';
    }
    size_t filelen = strlen(file);
    if (len + filelen >= dstsz) return 0;
    for (size_t i = 0; i < filelen; i++) dst[len + i] = file[i];
    dst[len + filelen] = '\0';
    return 1;
}

int carregar_dados(const char *pasta_csv, SistemaGestor *gestor) {
    char caminho_completo[1024];
    char caminho_erros[1024];

    if (!gestor || !pasta_csv) return 0;

    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "airports.csv")) return 0;

    const char *erros_rel = "resultados/airports_errors.csv";
    size_t l = strlen(erros_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_rel[i];
    caminho_erros[l] = '\0';

    int count_airports = csv_parse_file(
        caminho_completo,
        caminho_erros,
        gestor_global_get_aeroportos(gestor),
        NULL,
        NULL,
        criar_aeroporto,
        airport_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_airports < 0) return 0;

    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "aircrafts.csv")) return 0;
    const char *erros_aeronaves_rel = "resultados/aircrafts_errors.csv";
    l = strlen(erros_aeronaves_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_aeronaves_rel[i];
    caminho_erros[l] = '\0';

    int count_aeronaves = csv_parse_file(
        caminho_completo,
        caminho_erros,
        gestor_global_get_aeronaves(gestor),
        NULL,
        NULL,
        criar_aeronave,
        aeronave_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_aeronaves < 0) return 0;

    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "flights.csv")) return 0;
    const char *erros_voos_rel = "resultados/flights_errors.csv";
    l = strlen(erros_voos_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_voos_rel[i];
    caminho_erros[l] = '\0';

    int count_voos = csv_parse_file(
        caminho_completo,
        caminho_erros,
        gestor_global_get_voos(gestor),
        gestor,
        NULL,
        criar_voo,
        voo_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_voos < 0) return 0;

    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "passengers.csv")) return 0;
    const char *erros_passageiros_rel = "resultados/passengers_errors.csv";
    l = strlen(erros_passageiros_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_passageiros_rel[i];
    caminho_erros[l] = '\0';

    int count_passageiros = csv_parse_file(
        caminho_completo,
        caminho_erros,
        gestor_global_get_passageiros(gestor),
        NULL,
        NULL,
        criar_passageiro,
        passageiro_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_passageiros < 0) return 0;

    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "reservations.csv")) return 0;
    const char *erros_reservas_rel = "resultados/reservations_errors.csv";
    l = strlen(erros_reservas_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_reservas_rel[i];
    caminho_erros[l] = '\0';

    int count_reservas = csv_parse_file(
        caminho_completo,
        caminho_erros,
        gestor_global_get_reservas(gestor),
        gestor,
        NULL,
        criar_reserva,
        reserva_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_reservas < 0) return 0;

    return 1;
}

int executar_querry(char linha[], SistemaGestor *gestor, int *Nout) {
    if (linha[0] == '1') {
        execQ1(linha, gestor, Nout);
    } else if (linha[0] == '2') {
        execQ2(linha, gestor, Nout);
    } else if (linha[0] == '3') {
        execQ3(linha, gestor, Nout);
	} else if (linha[0] == '4') {
		execQ4(linha, gestor, Nout);
	} else if (linha[0] == '5') {
		execQ5(linha, gestor, Nout);
    }

    return 1;
}

int ler_inputs(const char *caminho_inputs, SistemaGestor *gestor, int *nOut) {
    FILE *inputs = fopen(caminho_inputs, "r");
    if (!inputs) return 0;
    int tamanho_linha = 256;
    char linha[tamanho_linha];

    while (fgets(linha, sizeof(linha), inputs)) {
        executar_querry(linha, gestor, nOut);
    }

    fclose(inputs);
    return 1;
}
