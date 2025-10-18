#include <voos.h>
#include <aeroportos.h>
#include <aeronaves.h>
#include <reservas.h>
#include <passageiros.h>
#include <parser.h>





int main(void) {
    AirportManager *gestor = airport_manager_create();

    int n = carregarficheiro_aeroporto(gestor, "trabalho-pratico/dataset-fase-1/sem_erros/airports.csv");

    printf("%d aeroportos válidos carregados.\n", n);

    airport_manager_print_summary(gestor);
    airport_manager_destroy(gestor);

    return 0;
}