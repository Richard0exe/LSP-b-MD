#include "fit_algorithm.h"

// First-fit algoritms

state_struct_ptr first_fit_state_init(const list *chunks);
bool first_fit_alloc(state_struct_ptr state, size_t size);
const fragmentation_info *first_fit_report_fragmentation(const state_struct_ptr state);
void first_fit_free_state(state_struct_ptr state);
