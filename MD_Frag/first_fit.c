#include "first_fit.h"
#include <stdlib.h>

typedef struct {
    list* chunks;
    size_t unallocated_bytes;
    size_t used_bytes;
} first_fit_state;

state_struct_ptr first_fit_state_init(const list *chunks) {
    first_fit_state* state = malloc(sizeof(first_fit_state));

    state->unallocated_bytes = 0;
    state->used_bytes = 0;

    if (chunks == NULL) {
        state->chunks = NULL;
        return state;
    }

    state->chunks = malloc(sizeof(list));
    state->chunks->value = chunks->value;
    state->chunks->next = NULL;

    list* current_chunk = state->chunks;
// Iterējam ar otro, jo pirmais jau ir inicializēts
    for (const list* chunk = chunks->next; chunk != NULL; chunk = chunk->next) {
        current_chunk->next = malloc(sizeof(list));
        current_chunk = current_chunk->next;

        current_chunk->value = chunk->value;
        current_chunk->next = NULL;
    }

    //Apvienojam pēdejo ar pirmo
    current_chunk->next = state->chunks;

    return state;
}

bool first_fit_alloc(state_struct_ptr state, size_t size) {
    first_fit_state* fit_state = state;

    if (fit_state->chunks == NULL) {
        fit_state->unallocated_bytes += size;
        return false;
    }

    list* current_chunk = fit_state->chunks;

    // Ejam cauri sarakstam līdz sastopam pirmo vietu, kur varam iesprausties.
    do {
        if (current_chunk->value >= size) {
            current_chunk->value -= size;
            fit_state->used_bytes += size;
            return true;
        }
        current_chunk = current_chunk->next;
    } while (current_chunk != fit_state->chunks);

    // Ja nav atrasts pietiekami liels chunk, tad pievienojam pie nealocētā
    fit_state->unallocated_bytes += size;

    return false;
}

const fragmentation_info* first_fit_report_fragmentation(const state_struct_ptr state) {
    first_fit_state* fit_state = state;
    fragmentation_info* tmp = malloc(sizeof(fragmentation_info));
    tmp->used_bytes = fit_state->used_bytes;
    tmp->unallocated_bytes = fit_state->unallocated_bytes;
    return tmp;
}

void first_fit_free_state(state_struct_ptr state) {
    first_fit_state* fit_state = state;

    if (fit_state->chunks == NULL) {
        free(state);
        return;
    }

    list* chunk = fit_state->chunks;
    list* next_chunk;

    do {
        next_chunk = chunk->next;
        free(chunk);
        chunk = next_chunk;
    } while (chunk != fit_state->chunks);

    free(state);
}
