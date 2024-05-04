#include "next_fit.h"
#include <stdlib.h>

typedef struct {
    list* chunks;
    list* current_chunk;
    size_t unallocated_bytes;
    size_t used_bytes;
} next_fit_state;

state_struct_ptr next_fit_state_init(const list* chunks) {
    next_fit_state* state = malloc(sizeof(next_fit_state));

    state->unallocated_bytes = 0;
    state->used_bytes = 0;

    if (chunks == NULL) {
        state->chunks = NULL;
        state->current_chunk = NULL;
        return state;
    }


    // Nokopē chunks iekšā state, lai varētu mainīt.
    state->chunks = malloc(sizeof(list));
    state->chunks->value = chunks->value;
    state->chunks->next = NULL;

    state->current_chunk = state->chunks;

    for (const list* chunk = chunks->next; chunk != NULL; chunk = chunk->next) {
        state->chunks->next = malloc(sizeof(list));
        state->chunks = state->chunks->next;

        state->chunks->value = chunk->value;
        state->chunks->next = NULL;
    }

    state->chunks->next = state->current_chunk;
    state->chunks = state->current_chunk;

    return state;
}

bool next_fit_alloc(state_struct_ptr state, size_t size) {
    next_fit_state* fit_state = state;

    if (fit_state->chunks == NULL) {
        fit_state->unallocated_bytes+=size;
        return false;
    }

    list* current_chunk = fit_state->current_chunk;

    // Iet līdz sastopas atkal ar sākumchunk, kurā gadījumā nav vietas.
    do {
        if (current_chunk->value >= size) {
            current_chunk->value-=size;

            fit_state->used_bytes+=size;
            fit_state->current_chunk = current_chunk;
            return true;
        }

        current_chunk = current_chunk -> next;
    } while (current_chunk != fit_state->current_chunk);

    fit_state->unallocated_bytes+=size;

    return false;
}

const fragmentation_info* next_fit_report_fragmentation(const state_struct_ptr state) {
    next_fit_state* fit_state = state;
    fragmentation_info *res = malloc(sizeof(fragmentation_info));
    res->used_bytes = fit_state->used_bytes;
    res->unallocated_bytes = fit_state->unallocated_bytes;
    return res;
}

void next_fit_free_state(state_struct_ptr state) {
    next_fit_state* fit_state = state;

    if (fit_state->chunks == NULL) {
        free(state);
        return;
    }

    list* chunk = fit_state->chunks;

    do {
        list* next_chunk = chunk->next;
        free(chunk);

        chunk = next_chunk;
    } while (chunk != fit_state->chunks);

    free(state);

    return;
}