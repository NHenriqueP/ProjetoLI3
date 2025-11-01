#include <voos.h>
#include <aeroportos.h>
#include <aeronaves.h>
#include <reservas.h>
#include <passageiros.h>
#include <csv_parser.h>
#include <stdio.h>
#include <stdlib.h> // Para snprintf 
#include <string.h> 

typedef struct 
{
    AeroportoManager *aeroportos;
    // aeronaves;
    //passageiros;
    //reservas;
    //voos;
} SistemaGestor;

void destruir_gestor_global(SistemaGestor *gestorGlobal) {
    if (!gestorGlobal) return;

    airport_manager_destroy(gestorGlobal->aeroportos); 

    free(gestorGlobal);

}

SistemaGestor *criar_gestor_global() {
    SistemaGestor *gestorGlobal = malloc(sizeof(SistemaGestor));
    if(gestorGlobal==NULL) return NULL;

    gestorGlobal->aeroportos = airport_manager_create();

    if(gestorGlobal->aeroportos == NULL) {
        destruir_gestor_global(gestorGlobal);
        return NULL;
    }

    return gestorGlobal;
}



int carregar_dados(const char *pasta_csv, SistemaGestor *gestor) {
    char caminho_completo[1024];
    int entidades_carregadas = 0;

    if (!gestor || !pasta_csv) return 0;

    snprintf(
        caminho_completo,
        sizeof(caminho_completo),
        "%s/%s",
        pasta_csv,
        "airports.csv"
    );

    int count_airports = csv_parser(
        caminho_completo,
        "resultados/airports_errors.csv",
        gestor->aeroportos,
        parse_airport_line,
        airport_manager_add_from_ptr,
        ','
    );

    if (count_airports < 0) return 0;

    entidades_carregadas += count_airports;
    //carregar voos,aeronaves etc

    return 1;
}

void executar_querry(char linha[],int N,SistemaGestor *gestor) {
    int i = 0;
    if (linha[0] == '1') {
        
    }
}

int ler_inputs(const char *caminho_inputs,SistemaGestor *gestor) {
    FILE *inputs = fopen(caminho_inputs,"r");
    if (!inputs) return 0;
    int tamanho_linha = 256;
    char linha[tamanho_linha]; 

    while(fgets(linha,sizeof(linha),inputs)) {
        executar_querry(linha,tamanho_linha,gestor);
    }

    fclose(inputs);
    return 1;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    SistemaGestor *gestorGlob = criar_gestor_global();

    const char *caminho_csv_pasta = argv[1]; // Caminho para a pasta
    const char *caminho_inputs = argv[2];    // Caminho para o ficheiro de comandos

    if (carregar_dados(caminho_csv_pasta,gestorGlob) == 0) {
        destruir_gestor_global(gestorGlob);
        return 1;
    };

    if (ler_inputs(caminho_inputs,gestorGlob) == 0) {
    }


    destruir_gestor_global(gestorGlob);

    return 0;
}