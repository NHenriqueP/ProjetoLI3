#ifndef APP_RUNNER_H
#define APP_RUNNER_H

#include "gestorglobal.h"

/**
 * @file app_runner.h
 * @brief Funções de orquestração da aplicação.
 *
 * Este módulo expõe as operações de alto nível usadas pela aplicação:
 * carregamento dos CSV para os gestores e execução de comandos (queries)
 * a partir de um ficheiro de inputs.
 */

/**
 * @brief Carrega os dados CSV para o sistema.
 *
 * Lê os ficheiros CSV esperados dentro da pasta indicada (por exemplo,
 * `airports.csv`, `aircrafts.csv`, `flights.csv`, `passengers.csv`,
 * `reservations.csv`) e insere as entidades nos respetivos gestores.
 *
 * Em caso de linhas inválidas, os módulos de parsing podem escrever as
 * linhas rejeitadas em ficheiros de erros na pasta `resultados/`.
 *
 * @param pasta_csv Caminho para a pasta onde se encontram os CSV.
 * @param gestor Gestor global previamente criado.
 * @return 1 em caso de sucesso; 0 em caso de erro.
 */
int carregar_dados(const char *pasta_csv, SistemaGestor *gestor);

/**
 * @brief Executa a query indicada numa linha de input.
 *
 * A função identifica a query a executar (por exemplo, pelo primeiro
 * carácter da linha) e delega para a função `execQ*` apropriada.
 *
 * @param linha Linha de comando lida do ficheiro de inputs.
 * @param gestor Gestor global com os dados carregados.
 * @param Nout Contador/índice de output a usar na escrita de resultados.
 * @return 1 se o comando for processado; 0 em caso de erro.
 */
int executar_querry(char linha[], SistemaGestor *gestor, int *Nout);

/**
 * @brief Lê um ficheiro de inputs e executa cada comando.
 *
 * Abre o ficheiro em `caminho_inputs`, lê linha a linha e chama
 * `executar_querry()` para cada linha.
 *
 * @param caminho_inputs Caminho para o ficheiro de inputs.
 * @param gestor Gestor global com os dados carregados.
 * @param nOut Contador/índice de output (atualizado pelas queries).
 * @return 1 em caso de sucesso; 0 se não for possível abrir/ler o ficheiro.
 */
int ler_inputs(const char *caminho_inputs, SistemaGestor *gestor, int *nOut);

#endif
