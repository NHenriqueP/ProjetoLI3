#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <glib.h>
#include "app_runner.h"
#include "gestorglobal.h"

#define RESULTS_DIR "resultados"

typedef struct {
    int total;
    int correct;
    double total_time;
} QueryStats;

static double elapsed_seconds(const struct timespec *start, const struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec) + (double)(end->tv_nsec - start->tv_nsec) / 1e9;
}

static int has_suffix(const char *name, const char *suffix) {
    size_t name_len = strlen(name);
    size_t suffix_len = strlen(suffix);
    if (name_len < suffix_len) return 0;
    return strcmp(name + name_len - suffix_len, suffix) == 0;
}

static void clean_results_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char full_path[512];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        if (!S_ISREG(st.st_mode)) continue;

        if (has_suffix(entry->d_name, ".txt") || has_suffix(entry->d_name, ".csv")) {
            remove(full_path);
        }
    }
    closedir(dir);
}

static int compare_files(const char *actual_path, const char *expected_path, int *first_diff_line) {
    FILE *actual = fopen(actual_path, "r");
    if (!actual) {
        *first_diff_line = 0;
        return -1;
    }
    FILE *expected = fopen(expected_path, "r");
    if (!expected) {
        fclose(actual);
        *first_diff_line = 0;
        return -1;
    }

    char *line_actual = NULL;
    char *line_expected = NULL;
    size_t len_actual = 0;
    size_t len_expected = 0;
    ssize_t read_actual;
    ssize_t read_expected;
    int line_no = 0;
    int status = 0;

    while (1) {
        read_actual = getline(&line_actual, &len_actual, actual);
        read_expected = getline(&line_expected, &len_expected, expected);
        line_no++;

        if (read_actual == -1 && read_expected == -1) {
            break;
        }
        if (read_actual == -1 || read_expected == -1) {
            status = 1;
            break;
        }
        if (read_actual != read_expected || memcmp(line_actual, line_expected, (size_t)read_actual) != 0) {
            status = 1;
            break;
        }
    }

    free(line_actual);
    free(line_expected);
    fclose(actual);
    fclose(expected);

    if (status == 0) {
        *first_diff_line = 0;
        return 0;
    }

    *first_diff_line = line_no;
    return 1;
}

int carregar_dados(const char *pasta_csv, SistemaGestor *gestor);
int executar_querry(char linha[], SistemaGestor *gestor, int *Nout);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <pasta_csv> <ficheiro_comandos> <pasta_resultados_esperados>\n", argv[0]);
        return 1;
    }

    const char *csv_dir = argv[1];
    const char *commands_path = argv[2];
    const char *expected_dir = argv[3];

    clean_results_directory(RESULTS_DIR);

    SistemaGestor *gestor = criar_gestor_global();
    if (!gestor) {
        fprintf(stderr, "Erro ao criar gestor global.\n");
        return 1;
    }

    if (!carregar_dados(csv_dir, gestor)) {
        fprintf(stderr, "Erro ao carregar dados a partir de %s.\n", csv_dir);
        destruir_gestor_global(gestor);
        return 1;
    }

    FILE *commands = fopen(commands_path, "r");
    if (!commands) {
        fprintf(stderr, "Erro ao abrir ficheiro de comandos %s: %s\n", commands_path, strerror(errno));
        destruir_gestor_global(gestor);
        return 1;
    }

    QueryStats stats[10] = {0};
    GArray *command_types = g_array_new(FALSE, FALSE, sizeof(int));
    if (!command_types) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        fclose(commands);
        destruir_gestor_global(gestor);
        return 1;
    }

    struct timespec global_start, global_end;
    clock_gettime(CLOCK_MONOTONIC, &global_start);

    char linha[512];
    int nOut = 1;
    while (fgets(linha, sizeof(linha), commands)) {
        size_t len = strlen(linha);
        while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r')) {
            linha[--len] = '\0';
        }

        char *trim = linha;
        while (*trim && isspace((unsigned char)*trim)) trim++;
        if (*trim == '\0') continue;

        int query_idx = -1;
        if (*trim >= '0' && *trim <= '9') {
            query_idx = *trim - '0';
            if (query_idx >= 0 && query_idx < 10) {
                stats[query_idx].total++;
            }
        }

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        int before_out = nOut;
        executar_querry(linha, gestor, &nOut);
        clock_gettime(CLOCK_MONOTONIC, &end);

        int produced_output = (nOut > before_out);
        if (produced_output) {
            int stored_idx = query_idx;
            g_array_append_val(command_types, stored_idx);
            if (query_idx >= 0 && query_idx < 10) {
                stats[query_idx].total_time += elapsed_seconds(&start, &end);
            }
        }
    }

    fclose(commands);
    clock_gettime(CLOCK_MONOTONIC, &global_end);

    double total_elapsed = elapsed_seconds(&global_start, &global_end);

    int total_commands = (int)command_types->len;
    for (int i = 1; i <= total_commands; i++) {
        char actual_path[256];
        char expected_path[512];
        snprintf(actual_path, sizeof(actual_path), RESULTS_DIR "/command%d_output.txt", i);
        snprintf(expected_path, sizeof(expected_path), "%s/command%d_output.txt", expected_dir, i);

        int diff_line = 0;
        int result = compare_files(actual_path, expected_path, &diff_line);
        int qidx = 0;
        if (i - 1 < (int)command_types->len) {
            qidx = g_array_index(command_types, int, i - 1);
        }

        if (result == 0) {
            if (qidx >= 0 && qidx < 10) stats[qidx].correct++;
        } else {
            if (result == -1) {
                printf("Command %d (Q%d): ficheiro em falta\n", i, qidx);
            } else {
                printf("Command %d (Q%d): primeira diferença na linha %d\n", i, qidx, diff_line);
            }
        }
    }

    printf("\nResumo por query:\n");
    for (int q = 0; q < 10; q++) {
        if (stats[q].total == 0) continue;
        double avg = stats[q].total > 0 ? stats[q].total_time / stats[q].total : 0.0;
        printf("Query %d -> corretos: %d/%d | tempo médio: %.6f s\n", q, stats[q].correct, stats[q].total, avg);
    }

    printf("Tempo total: %.6f s\n", total_elapsed);

    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        double rss_mb = usage.ru_maxrss / 1024.0; // ru_maxrss em KB
        printf("Memória máxima residente: %.2f MB\n", rss_mb);
    }

    g_array_free(command_types, TRUE);
    destruir_gestor_global(gestor);
    return 0;
}