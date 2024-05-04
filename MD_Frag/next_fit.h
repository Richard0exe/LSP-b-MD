#include "fit_algorithm.h"

// Next fit algoritms

state_struct_ptr next_fit_state_init(const list *chunks);
bool next_fit_alloc(state_struct_ptr state, size_t size);
const fragmentation_info *next_fit_report_fragmentation(const state_struct_ptr state);
void next_fit_free_state(state_struct_ptr state);