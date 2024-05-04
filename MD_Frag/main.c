#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fit_algorithm.h"
#include "no_alloc.h"
#include "next_fit.h"
#include "first_fit.h"

typedef struct {
    const char *chunks_file;
    const char *sizes_file;
} cmd_payload;

/**
 * Mēģina sapakot un atgriezt argumentus, un, ja tas neizdodās, atgriež NULL.
 */
cmd_payload *parse_args(int argc, char **argv) {
    int real_argc = argc - 1;

    if (real_argc != 4) return NULL;
    if (strcmp(argv[1], "-c")) return NULL;
    if (strcmp(argv[3], "-s")) return NULL;

    cmd_payload *res = malloc(sizeof(cmd_payload));
    res->chunks_file = argv[2];
    res->sizes_file = argv[4];
    return res;
}

/**
 * Pieņemot, ka `fp` ir fails ar veseliem skaitļiem, atgriež sarakstu ar šiem skaitļiem.
 *
 * - fp -- Fails, kas satur veselus skaitļus.
 */
list *file_to_list(FILE *fp) {
    list *res = NULL;
    list *end = NULL;

    int num;
    for (;;) {
        int num_read = fscanf(fp, "%d", &num);
        if (num_read == EOF || num_read == 0) break;

        list *node = malloc(sizeof(list));
        node->value = num;
        node->next = NULL;

        if (end) {
            end->next = node;
            end = node;
        }

        if (!res) {
            res = node;
            end = node; // Ja nebija res, nebija arī end
        }
    }

    return res;
}

typedef struct {
    const list *chunks;
    const list *sizes;
} chunks_sizes_data;

/**
 * Atgriež bloku un pieprasīto izmēru sarakstus.
 *
 * - payload -- parsēto argumentu struktūra.
 */
chunks_sizes_data *load_chunks_sizes_data(const cmd_payload *payload) {
    FILE *chunks_fp = fopen(payload->chunks_file, "r");

    if (!chunks_fp) {
        fprintf(stderr, "Kļūda atverot '%s'\n", payload->chunks_file);
        perror("");
        return NULL;
    }

    FILE *requests_fp = fopen(payload->sizes_file, "r");

    if (!requests_fp) {
        fprintf(stderr, "Kļūda atverot '%s'\n", payload->sizes_file);
        perror("");
        // release chunks
        return NULL;
    }

    chunks_sizes_data *res = malloc(sizeof(chunks_sizes_data));
    res->chunks = file_to_list(chunks_fp);
    res->sizes = file_to_list(requests_fp);
    return res;
}

clock_t measure_1k(const fit_algorithm *algorithm, const chunks_sizes_data *cs) {
    clock_t res = 0;

    state_struct_ptr state;
    const list *current_size;

    for (int i = 0; i < 1000; i++) {
        state = algorithm->state_init_f(cs->chunks);
        current_size = cs->sizes;

        clock_t start_time = clock();
        while (current_size) {
            algorithm->alloc_f(state, current_size->value);
            current_size = current_size->next;
        }
        res += clock() - start_time;
        algorithm->free_state_f(state);
    }

    return res;
}


int main(int argc, char **argv) {
    cmd_payload *payload = parse_args(argc, argv);

    // Ja argumentus neizdevās parsēt, tad tas nozīmē, ka lietotājs padeva argumentus
    // nepareizā formātā un ir jāpaziņo, kā lietot programmu.
    if (!payload) {
        fprintf(stderr, "Lietojums: md5 -c <chunks_file> -s <sizes_file>\n");
        return 1;
    }

    chunks_sizes_data *cs = load_chunks_sizes_data(payload);

    // load_chunks_sizes_data parūpējas par kļūdu izziņošanu, tāpēc var uzreiz atgiezt
    // un neko papildus nepaziņot.
    if (!cs) return 1;

    const fit_algorithm algorithms[] = {
            {
                    .name = "no alloc",
                    .state_init_f = no_alloc_state_init,
                    .alloc_f = no_alloc_alloc,
                    .report_fragmentation_f = no_alloc_report_fragmentation,
                    .free_state_f = no_alloc_free_state,
            },
            {
                    .name = "next fit",
                    .state_init_f = next_fit_state_init,
                    .alloc_f = next_fit_alloc,
                    .report_fragmentation_f = next_fit_report_fragmentation,
                    .free_state_f = next_fit_free_state,
            },
            {
                    .name = "first fit",
                    .state_init_f = first_fit_state_init,
                    .alloc_f = first_fit_alloc,
                    .report_fragmentation_f = first_fit_report_fragmentation,
                    .free_state_f = first_fit_free_state,
            }
            // visi pārējie algoritmi
    };

    // Piemēra kods, kā varētu tikt izmantots algoritms testēšanai
    for (size_t i = 0; i < sizeof(algorithms) / sizeof(*algorithms); i++) {
        const fit_algorithm current = algorithms[i];
        state_struct_ptr state = current.state_init_f(cs->chunks);
        const list *current_size = cs->sizes;

        while (current_size) {
            current.alloc_f(state, current_size->value);
            current_size = current_size->next;
        }

        const fragmentation_info *info = current.report_fragmentation_f(state);

        // Formātu skatīties 'test.py'
        printf("\"%s\" %lu %lu %lu %lu\n",
               current.name,
               info->used_bytes,
               FIT_BUFFER_SIZE - info->used_bytes,
               info->unallocated_bytes,
               measure_1k(&current, cs));

        free((void *)info);
        current.free_state_f(state);
    }
}