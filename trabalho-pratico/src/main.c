#include "app_runner.h"
#include "gestorglobal.h"

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