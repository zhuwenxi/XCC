#ifndef __NFA_H__
#define __NFA_H__
#endif

#include "util/hash_table.h"

typedef struct {
	int id;
} NFA_state_type;

typedef struct {
	// start & end state:
	NFA_state_type *start;
	array_list_type *end;

	// all states:
	array_list_type *states;

	// transfer diagram:
	hash_table_type *transfer_diagram;
} NFA_type;




/*
 * constructors && de-constructors:
 */

NFA_type *NFA_create();

bool NFA_deconstructor(NFA_type *self, va_list arg_list);
DECLARE_DESTROY(NFA)

NFA_state_type *NFA_state_create();

bool NFA_state_deconstructor(NFA_state_type *self);
DECLARE_DESTROY(NFA_state)

// construct a NFA from regexp str:
NFA_type *NFA_from_str(char *str);



/*
 * hash function for NFA_state:
 */
int NFA_state_hash(void *state);




/*
 * debug print function:
 */