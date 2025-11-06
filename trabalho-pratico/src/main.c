#include <voos.h>
#include <aeroportos.h>
#include <aeronaves.h>
#include <reservas.h>
#include <passageiros.h>
#include <csv_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "querie1.h"
#include "gestorglobal.h"
#include "querie2.h"


static int path_join2(char *dst, size_t dstsz, const char *dir, const char *file) {
    if (!dst || !dir || !file || dstsz == 0) return 0;
    dst[0] = '\0';
    size_t len = 0;
    size_t dirlen = strlen(dir);
    if (dirlen >= dstsz) return 0;
    // cópia com verificação prévia de limites (já feita acima)
    for (size_t i = 0; i < dirlen; i++) dst[i] = dir[i];
    dst[dirlen] = '\0';
    len = dirlen;
    if (len + 1 < dstsz && dst[len - (len>0)] != '/') {
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

    // ficheiro de erros na pasta de resultados
    
        const char *erros_rel = "resultados/airports_errors.csv";
        size_t l = strlen(erros_rel);
        if (l >= sizeof(caminho_erros)) return 0;
        for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_rel[i];
        caminho_erros[l] = '\0';
    

    int count_airports = csv_parser(
        caminho_completo,
        caminho_erros,
        gestor_global_get_aeroportos(gestor),
        parse_airport_line,
        airport_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_airports < 0) return 0;
       //printf("Aeroportos carregados: %d\n", count_airports);//colocar em comentário para não dar print no terminal
    // Carregar voos
    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "flights.csv")) return 0;
    const char *erros_voos_rel = "resultados/flights_errors.csv";
    l = strlen(erros_voos_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_voos_rel[i];
    caminho_erros[l] = '\0';

    int count_voos = csv_parser(
        caminho_completo,
        caminho_erros,
        gestor_global_get_voos(gestor),
        parse_voo_line,
        voo_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_voos < 0) return 0;

    // Carregar aeronaves
    if (!path_join2(caminho_completo, sizeof(caminho_completo), pasta_csv, "aircrafts.csv")) return 0;
    const char *erros_aeronaves_rel = "resultados/aircrafts_errors.csv";
    l = strlen(erros_aeronaves_rel);
    if (l >= sizeof(caminho_erros)) return 0;
    for (size_t i = 0; i < l; i++) caminho_erros[i] = erros_aeronaves_rel[i];
    caminho_erros[l] = '\0';

    int count_aeronaves = csv_parser(
        caminho_completo,
        caminho_erros,
        gestor_global_get_aeronaves(gestor),
        parse_aeronave_line,
        aeronave_ptr_manager_add_from_ptr_adapter,
        ','
    );
    if (count_aeronaves < 0) return 0;

    return 1;
}

int executar_querry(char linha[],SistemaGestor *gestor,int *Nout) {
    if (linha[0] == '1') {
        execQ1(linha,gestor,Nout);
    } else if (linha[0] == '2') {
        execQ2(linha,gestor,Nout);
    }
    return 1;
}

int ler_inputs(const char *caminho_inputs,SistemaGestor *gestor,int *nOut) {
    FILE *inputs = fopen(caminho_inputs,"r");
    if (!inputs) return 0;
    int tamanho_linha = 256;
    char linha[tamanho_linha]; 

    while(fgets(linha,sizeof(linha),inputs)) {
        executar_querry(linha,gestor,nOut);
    }

    fclose(inputs);
    return 1;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    int nOut = 1; // ficheiros devem começar em command1_output.txt
    SistemaGestor *gestorGlob = criar_gestor_global();

    const char *caminho_csv_pasta = argv[1]; // Caminho para a pasta
    const char *caminho_inputs = argv[2];    // Caminho para o ficheiro de comandos

    if (carregar_dados(caminho_csv_pasta,gestorGlob) == 0) {
        destruir_gestor_global(gestorGlob);
        return 1;
    };

    if (!ler_inputs(caminho_inputs,gestorGlob,&nOut)) {
        destruir_gestor_global(gestorGlob);
        return 1;
    }


    destruir_gestor_global(gestorGlob);

    return 0;
}