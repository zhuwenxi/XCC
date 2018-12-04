#include "util/DFA.h"
#include "util/util.h"
#include "util/queue.h"
#include "opts.h"
#include "NFA.h"
#include <time.h>

DFA_type *DFA_create()
{
	DFA_type *self = (DFA_type *)malloc(sizeof(DFA_type));

	self->transfer_diagram = hash_table_create(DFA_state_symbol_pair_hash);
	self->states = array_list_create();

	self->start = NULL;
	self->end = array_list_create();

	self->alphabet = NULL;

	self->nfa = NULL;
	
	return self;
}

bool
DFA_deconstructor(DFA_type *self, va_list arg_list)
{
	assert(self && self->transfer_diagram && self->states);

	// TO DO:
	hash_table_destroy(self->transfer_diagram, DFA_state_symbol_pair_deconstructor, NULL);
	array_list_destroy(self->states, DFA_state_deconstructor, NULL);
	array_list_destroy(self->end, NULL);

	if (self->alphabet)
	{
		linked_list_destroy(self->alphabet, char_deconstructor, NULL);
	}

	if (self->nfa)
	{
		NFA_destroy(self->nfa, NULL);
	}

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
DFA_state_deconstructor(DFA_state_type *state, va_list arg_list)
{
	assert(state);

	linked_list_destroy(state->nfa_states, NULL);
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
	string_buffer_destroy(self->symbol);
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
	hash_table_type *closure_map = hash_table_create(NFA_state_hash);

	int dummy_value = 123;

	//
	// Initial E(N) to N:
	//
	queue_type *work_queue = queue_create();
	linked_list_node_type *node = nfa_states->head;
	while (node) {
		NFA_state_type *nfa_state = node->data;
		assert(nfa_state);

		linked_list_insert_back(closure, nfa_state);
		hash_table_insert(closure_map, nfa_state, &dummy_value);

		enqueue(work_queue, nfa_state);

		node = node->next;
	}

	while (!queue_empty(work_queue)) {

		NFA_state_type *state = dequeue(work_queue);

		//linked_list_type *neighbor_list = linked_list_create();
		/*context_for_collect_direct_neighbor context;
		context.neighbor_list = neighbor_list;
		context.origin_state = state;*/

		//hash_table_traverse(trans_diag, _visitor_to_collect_direct_neighbor, &context);
		NFA_state_symbol_pair_type key;
		key.state = state;
		key.symbol = "EPSILON";

		array_list_type *neighbor_list = hash_table_search_all(trans_diag, &key, NFA_state_symbol_pair_compartor, NULL);

		int n_idx;
		for (n_idx = 0; n_idx < neighbor_list->length; ++n_idx)
		{
			NFA_state_type *nb_state = array_list_get(neighbor_list, n_idx);
			LOG(TRUE, "result: %p", hash_table_search(closure_map, nb_state, pointer_comparator, NULL));
			if (hash_table_search(closure_map, nb_state, pointer_comparator, NULL) == &dummy_value)
			{
				linked_list_insert_back(closure, nb_state);
				hash_table_insert(closure_map, nb_state, &dummy_value);
				enqueue(work_queue, nb_state);
			}
			// if (linked_list_search(closure, nb_state, pointer_comparator, NULL) == NULL) {
			// 	linked_list_insert_back(closure, nb_state);
			// 	enqueue(work_queue, nb_state);
			// }
		}

		array_list_destroy(neighbor_list, NULL);
	}

	queue_destroy(work_queue, NULL);

	return closure;
}

static void
DFA_state_renaming(DFA_type *dfa)
{
	int s_i;
	for (s_i = 0; s_i < dfa->states->length; s_i++) {
		NFA_state_type *state = array_list_get(dfa->states, s_i);
		state->id = s_i;
	}
}

static DFA_type *
subset_construction(NFA_type *nfa)
{
	LOG(DFA_LOG_ENABLE, "current NFA: %s", get_NFA_debug_str(nfa));

	DFA_type *dfa = DFA_create();

	linked_list_type *alphabet = _collect_alphabet(nfa);
	dfa->alphabet = alphabet;

	LOG(DFA_LOG_ENABLE, "alphabet: %s", get_linked_list_debug_str(alphabet, NULL));

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
	
	LOG(DFA_LOG_ENABLE, "initial_set: %s", get_linked_list_debug_str(dfa_start_set, NFA_state_debug_str, NULL));

	// hash_table_statistic(nfa->transfer_diagram);
	
	//
	// Initialize work queue.
	// 
	queue_type *work_queue = queue_create();
	enqueue(work_queue, dfa_start_state);

	while (!queue_empty(work_queue)) {
		int count = 0;

		DFA_state_type *source_dfa_state = dequeue(work_queue);
		linked_list_type *nfa_states = source_dfa_state->nfa_states;

		// for each symbol in dictionary
		linked_list_node_type *dict_node = alphabet->head;
		while (dict_node) {
			char *symbol = dict_node->data;

			linked_list_type *target_nfa_states = linked_list_create();
			linked_list_node_type *nfa_state_node = nfa_states->head;
			NFA_state_symbol_pair_type key;

			while (nfa_state_node) {
				++count;
				key.state = nfa_state_node->data;
				key.symbol = symbol;

				NFA_state_type *target = hash_table_search(nfa->transfer_diagram, &key, NFA_state_symbol_pair_compartor, NULL);

				if (target) {
					linked_list_insert_back(target_nfa_states, target);
				}

				nfa_state_node = nfa_state_node->next;
			}

			if (target_nfa_states->head == NULL) {
				dict_node = dict_node->next;
				linked_list_destroy(target_nfa_states, NULL);
				continue;
			}
			
			linked_list_type *origin_target_nfa_states = target_nfa_states;
			target_nfa_states = epsilon_closure(target_nfa_states, nfa->transfer_diagram);
			linked_list_destroy(origin_target_nfa_states, NULL);

			// check if "target_dfa_state" already exists.
			DFA_state_type *candidate_target_dfa_state = DFA_state_create();
			candidate_target_dfa_state->id = 0;
			candidate_target_dfa_state->nfa_states = target_nfa_states;

			DFA_state_type *target_dfa_state = NULL;
			array_list_node_type *target_dfa_state_node = array_list_search(dfa->states, candidate_target_dfa_state, DFA_state_compartor);
			
			if (!target_dfa_state_node) {
				// add "target_dfa_state" to DFA's "states"
				target_dfa_state = candidate_target_dfa_state;
				array_list_append(dfa->states, target_dfa_state);

				int i;
				for (i = 0; i < nfa->end->length; i++) {
					NFA_state_type *end_state = array_list_get(nfa->end, i);

					if (linked_list_search(target_dfa_state->nfa_states, end_state, NFA_state_compartor, NULL)) {
						array_list_append(dfa->end, target_dfa_state);
					}
				}
				

				// add "target_dfa_state" to work queue
				enqueue(work_queue, target_dfa_state);
			}
			else {
				target_dfa_state = target_dfa_state_node->data;
				linked_list_destroy(target_nfa_states, NULL);
				free(candidate_target_dfa_state);
			}

			// update DFA's "transfer_diagram"
			string_buffer key_symbol = string_buffer_create();
			string_buffer_append(&key_symbol, symbol);

			DFA_state_symbol_pair_type *dfa_key = DFA_state_symbol_pair_create(source_dfa_state, key_symbol);
			hash_table_insert(dfa->transfer_diagram, dfa_key, target_dfa_state);
			
			dict_node = dict_node->next;
		}
		//LOG(TRUE, "count: %d", count);
	}

	queue_destroy(work_queue, NULL);

	DFA_state_renaming(dfa);

	//char *db_str = ;
	LOG(DFA_LOG_ENABLE, "DFA: %s", get_DFA_debug_str(dfa));
	LOG(TRUE, "DFA states num: %d", dfa->states->length);
	//free(db_str);

	return dfa;
}

static int
state_in_which_partition(DFA_state_type *state, array_list_type *partition)
{
	/*LOG(TRUE, "which partition: %s\n", get_array_list_debug_str(partition, linked_list_debug_str, DFA_state_debug_str, NULL));
	LOG(TRUE, "state id: %d\n", state->id);*/
	int idx;
	for (idx = 0; idx < partition->length; ++idx)
	{
		linked_list_type *p = array_list_get(partition, idx);

		linked_list_node_type *ll_node = linked_list_search(p, state, DFA_state_compartor, NULL);

		if (ll_node) return idx;
	}

	return -1;
}

static array_list_type *
split(DFA_type *dfa, array_list_type *partition, linked_list_type *p) {
	array_list_type *ret = array_list_create();
	array_list_type *state_map = array_list_create();

	linked_list_type *alphabet = dfa->alphabet;

	linked_list_node_type *alphabet_node = alphabet->head;

	while (alphabet_node) {
		char *symbol = alphabet_node->data;

		LOG(DFA_MINIFY_LOG_ENABLE, "symbol: %s\n", symbol);
		// Clear "ret".
		array_list_destroy(state_map, linked_list_deconstructor, NULL);
		state_map = array_list_create();
		array_list_destroy(ret, linked_list_deconstructor, NULL);
		ret = array_list_create();

		// "ret" has the size of DFA state number plus one. The plus
		// one is for error state.
		array_list_adjust_length(state_map, dfa->states->length + 1);
		int array_idx;
		for (array_idx = 0; array_idx < dfa->states->length + 1; ++array_idx)
		{
			array_list_set(state_map, array_idx, linked_list_create());
		}

		linked_list_node_type *state_node;
		for (state_node = p->head; state_node != NULL; state_node = state_node->next) {
			DFA_state_type *state = state_node->data;

			DFA_state_symbol_pair_type key;
			key.state = state;
			key.symbol = symbol;

			LOG(DFA_MINIFY_LOG_ENABLE, "source state: %s", DFA_state_debug_str(state, NULL));

			DFA_state_type *target_state = hash_table_search(dfa->transfer_diagram, &key, DFA_state_symbol_pair_compartor, NULL);

			int partition_idx = -1;
			if (target_state)
			{
				LOG(DFA_MINIFY_LOG_ENABLE, "target state: %s", DFA_state_debug_str(target_state, NULL));

				partition_idx = state_in_which_partition(target_state, partition);
			}
			else
			{
				partition_idx = dfa->states->length;
			}

			LOG(DFA_MINIFY_LOG_ENABLE, "partition_idx: %d\n", partition_idx);
			assert(partition_idx >= 0);

			linked_list_type *ret_p = array_list_get(state_map, partition_idx);
			linked_list_insert_back(ret_p, state);
		}
		
		array_list_type *idx_array = array_list_create();
		for (array_idx = 0; array_idx < dfa->states->length + 1; ++array_idx)
		{
			
			linked_list_type *new_p = array_list_get(state_map, array_idx);
			if (new_p->head) {
				array_list_append(idx_array, create_int(array_idx));
			}
		}

		for (array_idx = 0; array_idx < idx_array->length; ++array_idx)
		{
			int state_map_idx = *TYPE_CAST(array_list_get(idx_array, array_idx), int *);
			linked_list_type *set = array_list_get(state_map, state_map_idx);
			array_list_append(ret, set);

			array_list_set(state_map, state_map_idx, NULL);

			LOG(DFA_MINIFY_LOG_ENABLE, "set: %s\n", get_array_list_debug_str(ret, linked_list_debug_str, DFA_state_debug_str, NULL));
		}

		if (idx_array->length > 1)
		{
			array_list_destroy(state_map, linked_list_deconstructor, NULL);
			array_list_destroy(idx_array, int_deconstructor, NULL);
			return ret;
		}
		else
		{
			array_list_destroy(idx_array, int_deconstructor, NULL);
		}

		alphabet_node = alphabet_node->next;
	}
	
	array_list_destroy(state_map, linked_list_deconstructor, NULL);

	return ret;
}

typedef struct {
	array_list_type *partition;
	array_list_type *states;
	hash_table_type *diag;
	hash_table_type *new_diag;
} gen_new_transfer_diagram_ctx;

static void 
_gen_new_transfer_diagram(void *key, void *value, void *context)
{
	DFA_state_symbol_pair_type *pair = key;
	DFA_state_type *source = pair->state;
	char *symbol = pair->symbol;

	gen_new_transfer_diagram_ctx *ctx = context;
	array_list_type *partition = ctx->partition;
	array_list_type *states = ctx->states;
	hash_table_type *diag = ctx->diag;
	hash_table_type *new_diag = ctx->new_diag;
	
	DFA_state_type *target = hash_table_search(diag, key, DFA_state_symbol_pair_compartor, NULL);
	assert(target);

	int target_state_id = state_in_which_partition(target, partition);
	DFA_state_type *new_target_state = array_list_get(states, target_state_id);
	assert(new_target_state);

	int source_state_id = state_in_which_partition(source, partition);
	DFA_state_type *new_source_state = array_list_get(states, source_state_id);
	assert(new_source_state);

	DFA_state_symbol_pair_type key_pair;
	key_pair.state = new_source_state;
	key_pair.symbol = symbol;

	// Add to the new transition table, if this transition does not exit.
	if (!hash_table_search(new_diag, &key_pair, DFA_state_symbol_pair_compartor, NULL))
	{
		string_buffer new_symbol = string_buffer_create();
		string_buffer_append(&new_symbol, symbol);

		DFA_state_symbol_pair_type *new_key_pair = DFA_state_symbol_pair_create(new_source_state, new_symbol);
		
		hash_table_insert(new_diag, new_key_pair, new_target_state);
	}
	
}

/*
 * Hopcroft's algorithm.
 * DFA => Minimal DFAs
 */
static DFA_type *
minify_DFA(DFA_type *dfa)
{
	LOG(DFA_MINIFY_LOG_ENABLE, "Start to minify DFA.");
	//
	// Initially, partition "P" is { { D[A] }, { D - D[A] } }
	//
	array_list_type *partition = array_list_create();

	linked_list_type *accept_states = linked_list_create();
	linked_list_type *non_accept_states = linked_list_create();

	int i;
	for (i = 0; i < dfa->states->length; ++i) {
		DFA_state_type *dfa_state = array_list_get(dfa->states, i);
		
		if (array_list_search(dfa->end, dfa_state, DFA_state_compartor, NULL)) {
			linked_list_insert_back(accept_states, dfa_state);
		}
		else {
			linked_list_insert_back(non_accept_states, dfa_state);
		}
	}

	array_list_append(partition, accept_states);
	array_list_append(partition, non_accept_states);

	LOG(DFA_LOG_ENABLE, "accept_states: %s", get_linked_list_debug_str(accept_states, DFA_state_debug_str, NULL));

	LOG(DFA_LOG_ENABLE, "non_accept_states: %s", get_linked_list_debug_str(non_accept_states, DFA_state_debug_str, NULL));

	// Hopcroft's algorithm, find a minimal parition with fix-point algorithm.
	int last_partition_size = 0;
	int cur_partition_size = partition->length;

	while (cur_partition_size != last_partition_size)
	{
		last_partition_size = cur_partition_size;

		array_list_type *new_partition = array_list_create();

		int partition_idx;
		for (partition_idx = 0; partition_idx < partition->length; ++partition_idx)
		{
			linked_list_type *p = array_list_get(partition, partition_idx);
			
			// Split p.
			LOG(DFA_MINIFY_LOG_ENABLE, "before split: %s\n", get_linked_list_debug_str(p, DFA_state_debug_str, NULL));

			array_list_type *split_set = split(dfa, partition, p);

			LOG(DFA_MINIFY_LOG_ENABLE, "split set: %s\n", get_array_list_debug_str(split_set, linked_list_debug_str, DFA_state_debug_str, NULL));

			// Add new partitions to "new_partition"
			int split_set_idx;
			for (split_set_idx = 0; split_set_idx < split_set->length; ++split_set_idx)
			{
				linked_list_type *new_p = array_list_get(split_set, split_set_idx);
				array_list_append(new_partition, new_p);

				array_list_set(split_set, split_set_idx, NULL);
			}
			array_list_destroy(split_set, NULL);
		}

		array_list_destroy(partition, linked_list_deconstructor, NULL);
		partition = new_partition;
		
		cur_partition_size = new_partition->length;
	}

	LOG(DFA_LOG_ENABLE, "Minimal partition: %s\n", get_array_list_debug_str(partition, linked_list_debug_str, DFA_state_debug_str, NULL));
	
	/*
	 * Construct a new DFA with the minimal partition we get.
	 */

	// New DFA's states.
	array_list_type *new_states = array_list_create();
	int part_idx;
	for (part_idx = 0; part_idx < partition->length; ++part_idx)
	{
		// Create a new state for each partition, state id is
		// the partition index.
		DFA_state_type *new_dfa_state = DFA_state_create();
		new_dfa_state->id = part_idx;
		new_dfa_state->nfa_states = linked_list_create();

		// Also retrieve all nfa states belong to this partition,
		// and copy to the new_dfa_state.
		linked_list_type *sub_partition = array_list_get(partition, part_idx);
		linked_list_node_type *sub_partition_node;
		for (sub_partition_node = sub_partition->head; sub_partition_node != NULL; sub_partition_node = sub_partition_node->next)
		{
			DFA_state_type *old_dfa_state = sub_partition_node->data;
			
			linked_list_type *nfas = old_dfa_state->nfa_states;
			linked_list_node_type *nfa_node;

			for (nfa_node = nfas->head; nfa_node != NULL; nfa_node = nfa_node->next)
			{
				assert(nfa_node->data);
				if (!linked_list_search(new_dfa_state->nfa_states, nfa_node->data, NFA_state_compartor, NULL))
				{
					linked_list_insert_back(new_dfa_state->nfa_states, nfa_node->data);
				}
			}
		}

		array_list_append(new_states, new_dfa_state);
	}

	// New DFA's start state.
	DFA_state_type *new_start_state = NULL;

	// New DFA's end states.
	array_list_type *new_end_states = array_list_create();
	
	// Set new DFA's start & end states.
	for (part_idx = 0; part_idx < partition->length; ++part_idx)
	{
		linked_list_type *sub_partition = array_list_get(partition, part_idx);

		// Flag to indicate current parition contains old DFA's end state.
		// Initially set to FALSE.
		bool has_end = FALSE;

		linked_list_node_type *sub_partition_node;
		for (sub_partition_node = sub_partition->head; sub_partition_node != NULL; sub_partition_node = sub_partition_node->next)
		{
			DFA_state_type *dfa_state = sub_partition_node->data;
			assert(dfa_state);

			// This partition contains old DFA's start state, mark it as
			// the new start state.
			if (DFA_state_compartor(dfa_state, dfa->start, NULL))
			{
				
				
				assert(part_idx < new_states->length);
				new_start_state = array_list_get(new_states, part_idx);
			}

			// This partition contains old DFA's end state, add it to the
			// the new end state list.
			if (array_list_search(dfa->end, dfa_state, DFA_state_compartor, NULL))
			{
				has_end = TRUE;
			}
		}

		if (has_end) {
			array_list_append(new_end_states, array_list_get(new_states, part_idx));
		}
	}

	assert(new_start_state);
	assert(new_end_states->length);

	LOG(DFA_MINIFY_LOG_ENABLE, "new_start_state: %s", DFA_state_debug_str(new_start_state, NULL));
	
	LOG(DFA_MINIFY_LOG_ENABLE, "new_end_states: %s", get_array_list_debug_str(new_end_states, DFA_state_debug_str, NULL));

	// New transition table.
	hash_table_type *new_transfer_diagram = hash_table_create(DFA_state_symbol_pair_hash);

	// Traverse the old transition table, to generate the new transition table entry.
	gen_new_transfer_diagram_ctx ctx;
	ctx.partition = partition;
	ctx.states = new_states;
	ctx.diag = dfa->transfer_diagram;
	ctx.new_diag = new_transfer_diagram;

	hash_table_traverse(dfa->transfer_diagram, _gen_new_transfer_diagram, &ctx);

	// De-construct old DFA's properties.
	// 1. No need to destroy "start", because it's a borrowed pointer;
	// 2. No need to destroy "alphabet", because it's reused in the
	//    new minified DFA.
	array_list_destroy(dfa->states, DFA_state_deconstructor, NULL);
	array_list_destroy(dfa->end, NULL);
	hash_table_destroy(dfa->transfer_diagram, DFA_state_symbol_pair_deconstructor, NULL);
	
	// De-construct partition.
	array_list_destroy(partition, linked_list_deconstructor, NULL);

	// Assign new properties to DFA to form the new minified DFA.
	dfa->states = new_states;
	dfa->start = new_start_state;
	dfa->end = new_end_states;
	dfa->transfer_diagram = new_transfer_diagram;

	LOG(DFA_MINIFY_LOG_ENABLE, "Minified DFA: \n%s", get_DFA_debug_str(dfa));

	return dfa;
}

/*
 * NFA to DFA through "subset construction"
 */
DFA_type *
NFA_to_DFA(NFA_type *nfa)
{
	DFA_type *dfa = subset_construction(nfa);
	minify_DFA(dfa);
	dfa->nfa = nfa;

	return dfa;
}

int
DFA_state_symbol_pair_hash(void *key)
{
	assert(key);

	DFA_state_symbol_pair_type *k = (DFA_state_symbol_pair_type *)key;

	int hash_code = 0;
	
	/*assert(k->state);
	linked_list_type *nfa_states = k->state->nfa_states;
	linked_list_node_type *nfa_state_node = NULL;

	for (nfa_state_node = nfa_states->head; nfa_state_node != NULL; nfa_state_node = nfa_state_node->next) {
		NFA_state_type *nfa_state = TYPE_CAST(nfa_state_node->data, NFA_state_type *);

		hash_code += nfa_state->id;
	}*/

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
	if (one == another) return TRUE;

	DFA_state_type *a = one;
	DFA_state_type *b = another;

	linked_list_type *a_nfa_states = a->nfa_states;
	linked_list_type *b_nfa_states = b->nfa_states;

	linked_list_node_type *a_node;
	linked_list_node_type *b_node;

	if (a_nfa_states == NULL && b_nfa_states != NULL) return FALSE;
	if (b_nfa_states == NULL && a_nfa_states != NULL) return FALSE;

	for (a_node = a_nfa_states->head, b_node = b_nfa_states->head; a_node && b_node; a_node = a_node->next, b_node = b_node->next) {
		int a_id = TYPE_CAST(a_node->data, NFA_state_type *)->id;
		int b_id = TYPE_CAST(b_node->data, NFA_state_type *)->id;

		if (a_id != b_id) return FALSE;
	}

	if (a_node != NULL || b_node != NULL) return FALSE;

	return TRUE;
}

char *
DFA_state_debug_str(DFA_state_type *state, va_list arg_list)
{
	//return get_linked_list_debug_str(state->nfa_states, NFA_state_debug_str, NULL);
	
	return my_itoa(state->id);
}

char *
get_DFA_debug_str(DFA_type *self)
{
	hash_table_type *diag = self->transfer_diagram;

	string_buffer debug_str = string_buffer_create();
	string_buffer_append(&debug_str, "digraph {");

	//
	// traverse the transfer diagram and generate digraph for each element.
	//
	array_list_type *buckets = diag->buckets;

	int i;
	for (i = 0; i < buckets->capacity; ++i) {
		array_list_node_type *bucket = buckets->content[i];

		if (bucket != NULL && bucket->data != NULL) {
			linked_list_node_type *ll_node = TYPE_CAST(bucket->data, linked_list_type *)->head;
			while (ll_node != NULL)  {
				hash_table_element_type *ele = ll_node->data;

				// (source state, symbol)
				DFA_state_symbol_pair_type *key = ele->key;
				DFA_state_type *source = key->state;
				char *symbol = key->symbol;

				string_buffer_append(&debug_str, "{");
				char *src_id_str = my_itoa(source->id);
				string_buffer_append(&debug_str, src_id_str);
				string_buffer_append(&debug_str, " [label=\"");
				string_buffer_append(&debug_str, src_id_str);
				free(src_id_str);
				string_buffer_append(&debug_str, "\"]}");
				string_buffer_append(&debug_str, " -> ");


				// (target state)
				DFA_state_type *target = ele->value;
				string_buffer_append(&debug_str, "{");
				char *tgt_id_str = my_itoa(target->id);
				string_buffer_append(&debug_str, tgt_id_str);
				string_buffer_append(&debug_str, " [label=\"");
				string_buffer_append(&debug_str, tgt_id_str);
				free(tgt_id_str);
				string_buffer_append(&debug_str, "\" ]} ");
				string_buffer_append(&debug_str, " [label=\"");
				string_buffer_append(&debug_str, symbol);
				string_buffer_append(&debug_str, "\"];");

				ll_node = ll_node->next;
			}
		}
	}

	// mark "start" and "end" states with special shape
	assert(self->start);
	char *start_id_str = my_itoa(self->start->id);
	string_buffer_append(&debug_str, start_id_str);
	free(start_id_str);

	string_buffer_append(&debug_str, " [shape=diamond];");

	//assert(self->end->length);
	for (i = 0; i < self->end->length; ++i) {
		DFA_state_type *end_state = array_list_get(self->end, i);

		char *end_id_str = my_itoa(end_state->id);
		string_buffer_append(&debug_str, end_id_str);
		free(end_id_str);

		string_buffer_append(&debug_str, " [shape=rectangle];");
	}
	string_buffer_append(&debug_str, " }");
	return debug_str;
}
