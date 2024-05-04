#include <stdlib.h>
#include "no_alloc.h"

typedef struct {
    const list *chunks;
    size_t unallocated_bytes;
} no_alloc_state;

state_struct_ptr no_alloc_state_init(const list *chunks) {
    no_alloc_state *res = malloc(sizeof(no_alloc_state));
    res->chunks = chunks;
    res->unallocated_bytes = 0;
    return res;
}

bool no_alloc_alloc(state_struct_ptr state, size_t size) {
    no_alloc_state *s = state;
    // Mēs nekad nealocējam, mēs tikai piefiksējam cik daudz baitus mums neizdevās alocēt
    // un uzreizam atgriežam false
    s->unallocated_bytes += size;
    return false;
}

const fragmentation_info *no_alloc_report_fragmentation(const state_struct_ptr state) {
    no_alloc_state *s = state;
    fragmentation_info *res = malloc(sizeof(fragmentation_info));
    res->used_bytes = 0; // Mēs nekad neizmantojam atmiņu
    res->unallocated_bytes = s->unallocated_bytes;
    return res;
}

void no_alloc_free_state(state_struct_ptr state) {
    free(state);
}