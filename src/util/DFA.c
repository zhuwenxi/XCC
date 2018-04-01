#include "util/DFA.h"
#include "util/util.h"
#include "util/queue.h"

bool
DFA_deconstructor(DFA_type *self, va_list arg_list)
{
	assert(self && self->transfer_diagram && self->states);

	// TO DO:
	hash_table_destroy(self->transfer_diagram, NFA_state_symbol_deconstructor, NULL);
	array_list_destroy(self->states, NFA_state_deconstructor, NULL);
	array_list_destroy(self->end, NULL);

	free(self);

	return TRUE;
}

DFA_state_type *
DFA_state_create()
{
	DFA_state_type *self = (DFA_state_type *)malloc(sizeof(DFA_state_type));
	self->id = 0;
	self->nfa_states = linked_list_create();

	return self;
}

bool
DFA_state_deconstructor(DFA_state_type *state)
{
	assert(state);

	linked_list_destroy(state->nfa_states, NFA_state_deconstructor, NULL);
	free(state);

	return TRUE;
}

static void
_visitor(void *key, void *value, void *context)
{
	NFA_state_symbol_pair_type *pair = key;
	char *symbol = pair->symbol;
	linked_list_type *alphabet = context;

	if (linked_list_search(alphabet, symbol, str_compartor, NULL) == NULL) {
		string_buffer s = string_buffer_create();
		string_buffer_append(&s, symbol);

		linked_list_insert_back(alphabet, s);
	}
}

static linked_list_type *
_collect_alphabet(NFA_type *nfa)
{
	linked_list_type *alphabet = linked_list_create();

	hash_table_traverse(nfa->transfer_diagram, _visitor, alphabet);

	return alphabet;
}

static linked_list_type *
epsilon_closure(linked_list_type *nfa_states)
{
	linked_list_type *closure = linked_list_create();

	//
	// Initial E(N) to N:
	//
	queue_type *work_queue = queue_create();
	linked_list_node_type *node = nfa_states->head;
	while (node) {
		NFA_state_type *nfa_state = node->data;
		assert(nfa_state);

		enqueue(work_queue, nfa_state);

		node = node->next;
	}

	while (queue_empty(work_queue)) {
		queue_node_type *q_node = dequeue(work_queue);
		assert(q_node && q_node->data);

		NFA_state_type *state = q_node->data;
	}

	

	return closure;
}

static DFA_type *
subset_construction(NFA_type *nfa)
{
	DFA_type *dfa = DFA_create();

	linked_list_type *alphabet = _collect_alphabet(nfa);
	LOG(TRUE, "alphabet: %s", get_linked_list_debug_str(alphabet, NULL));

	return dfa;
}

/*
 * NFA to DFA through "subset construction"
 */
DFA_type *
NFA_to_DFA(NFA_type *nfa)
{
	DFA_type *dfa = subset_construction(nfa);
}