#include "fit_algorithm.h"

// Testa algoritms no_alloc, kurš nekad neko nealocē

state_struct_ptr no_alloc_state_init(const list *chunks);
bool no_alloc_alloc(state_struct_ptr state, size_t size);
const fragmentation_info *no_alloc_report_fragmentation(const state_struct_ptr state);
void no_alloc_free_state(state_struct_ptr state);