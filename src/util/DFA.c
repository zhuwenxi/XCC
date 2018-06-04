#include "util/DFA.h"
#include "util/util.h"
#include "util/queue.h"

DFA_type *DFA_create()
{
	DFA_type *self = (DFA_type *)malloc(sizeof(DFA_type));

	self->transfer_diagram = hash_table_create(DFA_state_symbol_pair_hash);
	self->states = array_list_create();

	self->start = NULL;
	self->end = array_list_create();

	return self;
}

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
	self->nfa_states = NULL;

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

DFA_state_symbol_pair_type *
DFA_state_symbol_pair_create(DFA_state_type *state, char *symbol)
{
	DFA_state_symbol_pair_type *self = (DFA_state_symbol_pair_type *)malloc(sizeof(DFA_state_symbol_pair_type));
	self->state = state;
	self->symbol = symbol;

	return self;
}

bool
DFA_state_symbol_pair_deconstructor(DFA_state_symbol_pair_type *self, va_list arg_list)
{
	string_buffer_destroy(self->symbol, NULL);
	free(self);

	return TRUE;
}

static void
_visitor(void *key, void *value, void *context)
{
	NFA_state_symbol_pair_type *pair = key;
	char *symbol = pair->symbol;
	linked_list_type *alphabet = context;

	if (linked_list_search(alphabet, symbol, str_compartor, NULL) == NULL && strcmp(symbol, "EPSILON") != 0) {
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

typedef struct {
	linked_list_type *neighbor_list;
	NFA_state_type *origin_state;
} context_for_collect_direct_neighbor;

static void 
_visitor_to_collect_direct_neighbor(void *key, void *value, void *context)
{
	context_for_collect_direct_neighbor *c = TYPE_CAST(context, context_for_collect_direct_neighbor *);
	linked_list_type *neighbor_list = c->neighbor_list;
	NFA_state_type *origin_state = c->origin_state;

	NFA_state_symbol_pair_type *state_symbol_pair = TYPE_CAST(key, NFA_state_symbol_pair_type *);
	NFA_state_type *source_state = state_symbol_pair->state;
	char *symbol = state_symbol_pair->symbol;

	NFA_state_type *target_state = TYPE_CAST(value, NFA_state_type *);

	if (strcmp(symbol, "EPSILON") == 0 && source_state == origin_state) {
		linked_list_insert_back(neighbor_list, target_state);
	}

}

static linked_list_type *
epsilon_closure(linked_list_type *nfa_states, hash_table_type *trans_diag)
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

		linked_list_insert_back(closure, nfa_state);
		enqueue(work_queue, nfa_state);

		node = node->next;
	}

	while (!queue_empty(work_queue)) {

		NFA_state_type *state = dequeue(work_queue);

		linked_list_type *neighbor_list = linked_list_create();
		context_for_collect_direct_neighbor context;
		context.neighbor_list = neighbor_list;
		context.origin_state = state;

		hash_table_traverse(trans_diag, _visitor_to_collect_direct_neighbor, &context);

		linked_list_node_type *nb_list_node = neighbor_list->head;
		while (nb_list_node) {
			NFA_state_type *nb_state = nb_list_node->data;
			if (linked_list_search(closure, nb_state, pointer_comparator, NULL) == NULL) {
				linked_list_insert_back(closure, nb_state);
				enqueue(work_queue, nb_state);
			}
			nb_list_node = nb_list_node->next;
		}
	}

	

	return closure;
}

static DFA_type *
subset_construction(NFA_type *nfa)
{
	LOG(TRUE, "current NFA: %s", get_NFA_debug_str(nfa, NULL));
	DFA_type *dfa = DFA_create();

	linked_list_type *alphabet = _collect_alphabet(nfa);
	LOG(TRUE, "alphabet: %s", get_linked_list_debug_str(alphabet, NULL));

	linked_list_type *nfa_start = linked_list_create();
	linked_list_insert_back(nfa_start, nfa->start);

	//
	// DFA's start state.
	//
	linked_list_type *dfa_start_set = epsilon_closure(nfa_start, nfa->transfer_diagram);
	DFA_state_type *dfa_start_state = DFA_state_create();
	dfa_start_state->nfa_states = dfa_start_set;
	dfa->start = dfa_start_state;
	array_list_append(dfa->states, dfa_start_state);

	linked_list_destroy(nfa_start, NULL);
	
	LOG(TRUE, "initial_set: %s", get_linked_list_debug_str(dfa_start_set, NFA_state_debug_str, NULL));

	
	//
	// Initialize work queue.
	// 
	queue_type *work_queue = queue_create();
	enqueue(work_queue, dfa_start_state);

	while (!queue_empty(work_queue)) {
		DFA_state_type *source_dfa_state = dequeue(work_queue);
		linked_list_type *nfa_states = source_dfa_state->nfa_states;

		// for each symbol in dictionary
		linked_list_node_type *dict_node = alphabet->head;
		while (dict_node) {
			char *symbol = dict_node->data;

			LOG(TRUE, "symbol: %s", symbol);

			linked_list_type *target_nfa_states = linked_list_create();
			linked_list_node_type *nfa_state_node = nfa_states->head;
			NFA_state_symbol_pair_type key;

			while (nfa_state_node) {
				key.state = nfa_state_node->data;
				key.symbol = symbol;

				LOG(TRUE, "search NFA states: ");
				LOG(TRUE, "key.symbol: %s", key.symbol);
				LOG(TRUE, "key.state: %s", NFA_state_debug_str(key.state, NULL));
				NFA_state_type *target = hash_table_search(nfa->transfer_diagram, &key, NFA_state_symbol_pair_compartor, NULL);

				if (target) {
					LOG(TRUE, "target.state: %s", NFA_state_debug_str(target, NULL));
					linked_list_insert_back(target_nfa_states, target);
				}

				nfa_state_node = nfa_state_node->next;
			}

			if (target_nfa_states->head == NULL) {
				LOG(TRUE, "no target nfa states");
				dict_node = dict_node->next;
				continue;
			}
			
			linked_list_type *origin_target_nfa_states = target_nfa_states;
			LOG(TRUE, "before epsilon closure: %s", get_linked_list_debug_str(origin_target_nfa_states, NFA_state_debug_str, NULL));
			target_nfa_states = epsilon_closure(target_nfa_states, nfa->transfer_diagram);
			LOG(TRUE, "after epsilon closure: %s", get_linked_list_debug_str(target_nfa_states, NFA_state_debug_str, NULL));
			linked_list_destroy(origin_target_nfa_states, NULL);

			// check if "target_dfa_state" already exists.
			DFA_state_type *candidate_target_dfa_state = DFA_state_create();
			candidate_target_dfa_state->id = 0;
			candidate_target_dfa_state->nfa_states = target_nfa_states;

			DFA_state_type *target_dfa_state = array_list_search(dfa->states, candidate_target_dfa_state, DFA_state_compartor);

			if (!target_dfa_state) {
				// add "target_dfa_state" to DFA's "states"
				target_dfa_state = candidate_target_dfa_state;
				array_list_append(dfa->states, target_dfa_state);

				LOG(TRUE, "new dfa states: %s", DFA_state_debug_str(target_dfa_state, NULL));
				// add "target_dfa_state" to work queue
				enqueue(work_queue, target_dfa_state);
			}
			else {
				free(candidate_target_dfa_state);
			}

			// update DFA's "transfer_diagram"
			string_buffer key_symbol = string_buffer_create();
			string_buffer_append(&key_symbol, symbol);

			DFA_state_symbol_pair_type *dfa_key = DFA_state_symbol_pair_create(source_dfa_state, key_symbol);
			hash_table_insert(dfa->transfer_diagram, dfa_key, target_dfa_state);
			
			dict_node = dict_node->next;
		}
	}

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

int
DFA_state_symbol_pair_hash(void *key)
{
	assert(key);

	DFA_state_symbol_pair_type *k = (DFA_state_symbol_pair_type *)key;

	int hash_code = 0;
	
	assert(k->state);
	linked_list_type *nfa_states = k->state->nfa_states;
	linked_list_node_type *nfa_state_node = NULL;

	for (nfa_state_node = nfa_states->head; nfa_state_node != NULL; nfa_state_node = nfa_state_node->next) {
		NFA_state_type *nfa_state = TYPE_CAST(nfa_state_node->data, NFA_state_type *);

		hash_code += nfa_state->id;
	}

	hash_code += *(k->symbol);

	return hash_code;
}

bool
DFA_state_symbol_pair_compartor(void *one, void *another, va_list arg_list)
{
	DFA_state_symbol_pair_type *a = one;
	DFA_state_symbol_pair_type *b = another;

	assert(a && b);

	char a_symbol = *(a->symbol);
	char b_symbol = *(b->symbol);

	if (a_symbol != b_symbol) return FALSE;

	return DFA_state_compartor(a->state, b->state, NULL);
}

bool
DFA_state_compartor(void *one, void *another, va_list arg_list)
{
	DFA_state_type *a = one;
	DFA_state_type *b = another;

	linked_list_type *a_nfa_states = a->nfa_states;
	linked_list_type *b_nfa_states = b->nfa_states;

	linked_list_node_type *a_node;
	linked_list_node_type *b_node;

	for (a_node = a_nfa_states->head, b_node = b_nfa_states->head; a_node && b_node; a_node = a_node->next, b_node = b_node->next) {
		int a_id = TYPE_CAST(a_node->data, NFA_state_type *)->id;
		int b_id = TYPE_CAST(b_node->data, NFA_state_type *)->id;

		if (a_id != b_id) return FALSE;
	}

	if (a_node != NULL || b_node != NULL) return FALSE;

	return FALSE;
}

char *
DFA_state_debug_str(DFA_state_type *state, va_list arg_list)
{
	return get_linked_list_debug_str(state->nfa_states, NFA_state_debug_str, NULL);
}

