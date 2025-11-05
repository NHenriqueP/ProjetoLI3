#include "querie1.h"
#include <string.h>
#include "airportos.h"

void execQ1(char linha[], int N, SistemaGestor *gestor, int *Nout) {
    char codigo_iata[5];

    if (linha[0] != '1' || linha[1] == '\0') {
        // Não é a Query 1 ou a linha está muito curta
        return; 
    }

    char *p = &linha[1]; // Ponto de partida após o '1'

    while (*p != '\0' && *p == ' ') p++; // Ignorar espaços

    if (*p != '\0') {
        size_t len = 0;
        size_t max_len = 4;
        char *ini = p;
        
        while (len < max_len && *p != '\0' && !isspace((unsigned char)*p)) {
            p++;
            len++;
        }
        
        if (len == 3 || len == 4) {
            // strncpy copia 'len' caracteres de 'ini' para 'codigo_iata'
            strncpy(codigo_iata, ini, len);
            
            // 5. Garantir a terminação nula (o strncpy não faz isso automaticamente se len = 4)
            codigo_iata[len] = '\0'; 

            char caminho_output[256]; // Buffer de tamanho suficiente

            snprintf(
                caminho_output,
                sizeof(caminho_output),
                "resultados/command%d_output.txt",
                *Nout
            );

            FILE *output_file = fopen(caminho_output, "w");
            if (!output_file) { 
                return; 
            }

            Aeroporto *aeroporto_ptr = airport_manager_find_by_iata(gestor->aeroportos, codigo_iata);
            if (aeroporto_ptr != NULL) {
                fprintf(output_file, "%s,%s,%s,%s,%s", aeroporto_ptr->iata, aeroporto_ptr->nome, aeroporto_ptr->cidade, aeroporto_ptr->pais, aeroporto_ptr->tipo);
            }
            else {
                fprintf(output_file,"\n");
            }
            fclose(output_file);
            (*Nout)++;

            return;
        }
    }
    

}