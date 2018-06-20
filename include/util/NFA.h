#ifndef __NFA_H__
#define __NFA_H__


#include "util/hash_table.h"
#include "util/util.h"

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

typedef struct {
	NFA_state_type *state;
	char *symbol;
} NFA_state_symbol_pair_type;




/*
 * constructors && de-constructors:
 */

NFA_type *NFA_create();

bool NFA_deconstructor(NFA_type *self, va_list arg_list);
DECLARE_DESTROY(NFA)

NFA_state_type *NFA_state_create();

bool NFA_state_deconstructor(NFA_state_type *self, va_list arg_list);
DECLARE_DESTROY(NFA_state)

bool NFA_state_compartor(void *one, void *another, va_list arg_list);

NFA_state_symbol_pair_type *NFA_state_symbol_pair_create();
bool NFA_state_symbol_deconstructor(NFA_state_symbol_pair_type *self, va_list arg_list);
DECLARE_DESTROY(NFA_state_symbol_pair)

// construct a NFA from regexp str:
NFA_type *NFA_from_str(char *str);



/*
 * hash function for NFA_state:
 */
int NFA_state_symbol_pair_hash(void *key);


/*
 * interfaces:
 */
bool NFA_state_symbol_pair_compartor(void *one, void *another, va_list arg_list);




/*
 * debug print function:
 */
char *get_NFA_debug_str(NFA_type *self);

char *NFA_state_debug_str(NFA_state_type *state, va_list arg_list);

#endif