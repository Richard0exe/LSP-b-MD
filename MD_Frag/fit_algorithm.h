#include <stdbool.h>
#include <stddef.h>

/**
 * fit_algorithm -- Alokācijas algoritmu interfeiss.
 *
 * Definē, kas ir jāatbalsta alokācijas algoritmam, lai to varētu pēc tam ērti un efektīvi
 * mērīt un testēt. Alokācijas algoritma izstrādātājs nodod savu algoritmu sapakotu
 * `fit_algorithm` struktūrā.
 *
 * Algoritms būs izmantots sekojošā veidā Pieņemot, ka `algo` ir fit_algorithm struktūra
 * 1. No sākuma tiks iegūts stāvoklis, izsaucot `algo.state_init_f`.
 * 2. Šis stāvoklis, kopā ar kādu izmēru tiks izmantots, lai izsauktu algo.alloc_f 0 vai
 *    vairākas reizes.
 * 3. Tad tiek izsaukts `algo.report_fragmentation_f`, lai iegūtu fragmentācijas datus
 * 4. Visbeidzot tiek izsaukts `algo.free_state_f` un ar to algoritma izmantošana tiek beigta.
 */

#ifndef FIT_ALGORITHM_H
#define FIT_ALGORITHM_H

/**
 * Pieejamais baitu skaits alocētājalgoritmam.
 */
#define FIT_BUFFER_SIZE 1024

typedef struct _list {
    size_t value;
    struct _list *next;
} list;

/**
 * Norāde uz alocētājalgoritma stāvokli.
 */
typedef void* state_struct_ptr;

/**
 * Izveido un atgriež alocētājalgoritma sākumstāvokli.
 *
 * - chunks -- saraksts ar pieejamiem caurumiem.
 */
typedef state_struct_ptr(*fit_state_init_f)(const list *chunks);

/**
 * Mēģina alocēt atmiņu ar izmēru `size`. Ja izdodas, atgriež true, citādi atgriež false.
 *
 * - state -- alocētājalgoritma stāvoklis.
 * - size -- pieprasītais atmiņas izmērs baitos.
 */
typedef bool(*fit_alloc_f)(state_struct_ptr state, size_t size);

/**
 * Struktūra, kas nodrošina vienotu formātu, lai paziņotu par fragmentāciju.
 */
typedef struct {
    size_t used_bytes; // Cik baiti tika izmantoti
    size_t unallocated_bytes; // Cik baitus neizdevās alocēt
} fragmentation_info;

/**
 * Izveido informāciju par alocētajalgoritma fragmentāciju.
 *
 * Šī informācija tiks atbrīvota automātiski, pašam to nedrīkst atbrīvot.
 *
 * - state -- alocētājalgoritma stāvoklis.
 */
typedef const fragmentation_info *(*fit_report_fragmentation_f)(const state_struct_ptr state);

/**
 * Atbrīvo alocētājalgoritma stāvokli.
 *
 * Pārsvarā ir jāatbrīvo tikai pats `state`, bet ja alocētājalgoritma darbības laika tika
 * dinamiska veidota atmiņa, tad par to ir šeit jāparūpējās.
 */
typedef void (*fit_free_state_f)(state_struct_ptr state);

/**
 * Formāts, kurā jāpadod alocētājalgoritms.
 */
typedef struct {
    const char *name; // Cilvēkam saprotams algoritma nosaukums
    fit_state_init_f state_init_f;
    fit_alloc_f alloc_f;
    fit_report_fragmentation_f report_fragmentation_f;
    fit_free_state_f free_state_f;
} fit_algorithm;

#endif