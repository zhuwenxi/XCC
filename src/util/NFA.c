#include <stdlib.h>

#include "util/NFA.h"
#include "util/Ast.h"
#include "util/LR_automata.h"
#include "util/regexp.h"
#include "util/logger.h"
#include "opts.h"

NFA_type *NFA_create()
{
	NFA_type *self = (NFA_type *)malloc(sizeof(NFA_type));

	self->transfer_diagram = hash_table_create(NFA_state_symbol_pair_hash);
	self->states = array_list_create();

	self->start = NULL;
	self->end = array_list_create();

	return self;
}

bool NFA_deconstructor(NFA_type *self, va_list arg_list)
{
	assert(self && self->transfer_diagram && self->states);

	// TO DO:
	hash_table_destroy(self->transfer_diagram, NFA_state_symbol_deconstructor, NULL);
	array_list_destroy(self->states, NFA_state_deconstructor, NULL);
	array_list_destroy(self->end, NULL);

	free(self);

	return TRUE;
}

NFA_state_type *NFA_state_create()
{
	NFA_state_type *self = (NFA_state_type *)malloc(sizeof(NFA_state_type));

	// Initial "id" is 0.
	self->id = 0;

	return self;
}

bool NFA_state_deconstructor(NFA_state_type *self)
{
	free(self);
	
	return TRUE;
}

NFA_state_symbol_pair_type *
NFA_state_symbol_pair_create(NFA_state_type *state, char *symbol)
{
	NFA_state_symbol_pair_type *self = (NFA_state_symbol_pair_type *)malloc(sizeof(NFA_state_symbol_pair_type));
	self->state = state;
	self->symbol = symbol;

	return self;
}

bool
NFA_state_symbol_deconstructor(NFA_state_symbol_pair_type *self, va_list arg_list)
{
	string_buffer_destroy(self->symbol, NULL);
	free(self);

	return TRUE;
}

static Ast_type*
AST_from_str(char *str)
{
	//
	// Initialize LR automata:
	//

	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, REGEXP, REGEXP, VERTICAL_BAR, CONCAT, 0);
	context_free_grammar_add(cfg, REGEXP, CONCAT, 0);

	context_free_grammar_add(cfg, CONCAT, CONCAT, REPEAT, 0);
	context_free_grammar_add(cfg, CONCAT, REPEAT, 0);

	context_free_grammar_add(cfg, REPEAT, UNIT, STAR, 0);
	context_free_grammar_add(cfg, REPEAT, UNIT, 0);

	context_free_grammar_add(cfg, UNIT, LEFT_PARENTHESIS, REGEXP, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, UNIT, CHAR, 0);

	LR_automata_type *lr_automata = LR_automata_create(cfg);

	//
	// Prepare input buffer:
	//

	LR_automata_input_buffer_type *buffer = LR_automata_input_buffer_create();
	LR_automata_input_buffer_init(buffer, str, regexp_grammar_get_token_type);

	//
	// Parsing:
	//
	Ast_type *ast = LR_automata_parse(lr_automata, buffer, construct_ast_for_regexp_grammar);

	LR_automata_input_buffer_destroy(buffer, NULL);

	LR_automata_destroy(lr_automata, NULL);

	return ast;
}

static void
NFA_state_renaming(NFA_type *nfa)
{
	int s_i;
	for (s_i = 0; s_i < nfa->states->length; s_i++) {
		NFA_state_type *state = array_list_get(nfa->states, s_i);
		state->id = s_i;
	}
}

static void
merge_nfas(array_list_type *nfas, NFA_type *big_nfa)
{
	assert(nfas && big_nfa);

	int i;
	for (i = 0; i < nfas->length; i++) {
		NFA_type *nfa = array_list_get(nfas, i);
		assert(nfa);

		//
		// merge states to the big NFA.
		//

		// merge states:
		int s_i;
		for (s_i = 0; s_i < nfa->states->length; s_i++) {
			NFA_state_type *state = array_list_get(nfa->states, s_i);
			array_list_append(big_nfa->states, state);

			 state->id = big_nfa->states->length - 1;
		}

		// update transfer diagram:
		hash_table_type *sub_trans_diag = nfa->transfer_diagram;
		int bucket_index;
		for (bucket_index = 0; bucket_index < sub_trans_diag->buckets->capacity; bucket_index++) {
			array_list_node_type *bucket = sub_trans_diag->buckets->content[bucket_index];
			if (bucket != NULL && bucket->data != NULL) {
				linked_list_type *ll = bucket->data;
				linked_list_node_type *ll_node = ll->head;

				while (ll_node) {
					hash_table_element_type *ele = ll_node->data;
					NFA_state_symbol_pair_type *key = ele->key;
					NFA_state_type *value = ele->value;

					// update in big NFA's transfer diagram:
					hash_table_insert(big_nfa->transfer_diagram, key, value);
					
					ll_node = ll_node->next;
				}
			}
		}

		// destroy the sub-NFA. (have to be partial destroyed, since some objects are still refered by the big NFA)
		nfa->start = NULL;
		array_list_destroy(nfa->end, NULL);
		array_list_destroy(nfa->states, NULL);
		hash_table_destroy(nfa->transfer_diagram, NULL);
		free(nfa);
	}

	//NFA_state_renaming(big_nfa);
}

static NFA_type *
merge_NFA_for_concat(array_list_type *nfas)
{
	// because "concat" is a binary operator
	assert(nfas->length == 2);

	// the NFA of left sub-node must have only one "end" state
	NFA_type *left_nfa = TYPE_CAST(array_list_get(nfas, 0), NFA_type *);
	assert(left_nfa->end->length == 1);

	NFA_type *right_nfa = TYPE_CAST(array_list_get(nfas, 1), NFA_type *);
	

	// one big new NFA, to subsititude the "left" and "right" NFAs.
	NFA_type *new_nfa = NFA_create();

	new_nfa->start = left_nfa->start;
	array_list_append(new_nfa->end, array_list_get(right_nfa->end, 0));
	
	string_buffer epsilon = string_buffer_create();
	string_buffer_append(&epsilon, "EPSILON");
	NFA_state_symbol_pair_type *pair = NFA_state_symbol_pair_create(array_list_get(left_nfa->end, 0), epsilon);
	hash_table_insert(new_nfa->transfer_diagram, pair, right_nfa->start);
	merge_nfas(nfas, new_nfa);

	return new_nfa;
}

static NFA_type *
merge_NFA_for_alter(array_list_type *nfas)
{
	// because "alter" is a binary operator
	assert(nfas->length == 2);

	//left & right sub-node must have only one "end" state
	NFA_type *left_nfa = TYPE_CAST(array_list_get(nfas, 0), NFA_type *);
	assert(left_nfa->end->length == 1);

	NFA_type *right_nfa = TYPE_CAST(array_list_get(nfas, 1), NFA_type *);
	assert(right_nfa->end->length == 1);

	// one big new NFA, to subsititude the "left" and "right" NFAs.
	NFA_type *new_nfa = NFA_create();

	//
	// new "start" and "end" states for the big NFA:
	//

	NFA_state_type *new_start = NFA_state_create();
	array_list_append(new_nfa->states, new_start);
	new_nfa->start = new_start;

	NFA_state_type *new_end = NFA_state_create();
	array_list_append(new_nfa->states, new_end);
	array_list_append(new_nfa->end, new_end);

	//
	// update transfer diagram:
	//

	string_buffer epsilon1 = string_buffer_create();
	string_buffer_append(&epsilon1, "EPSILON");

	string_buffer epsilon2 = string_buffer_create();
	string_buffer_append(&epsilon2, "EPSILON");

	string_buffer epsilon3 = string_buffer_create();
	string_buffer_append(&epsilon3, "EPSILON");

	string_buffer epsilon4 = string_buffer_create();
	string_buffer_append(&epsilon4, "EPSILON");

	NFA_state_symbol_pair_type *pair1 = NFA_state_symbol_pair_create(new_start, epsilon1);
	hash_table_insert(new_nfa->transfer_diagram, pair1, left_nfa->start);

	NFA_state_symbol_pair_type *pair2 = NFA_state_symbol_pair_create(new_start, epsilon2);
	hash_table_insert(new_nfa->transfer_diagram, pair2, right_nfa->start);

	NFA_state_symbol_pair_type *pair3 = NFA_state_symbol_pair_create(array_list_get(left_nfa->end, 0), epsilon3);
	hash_table_insert(new_nfa->transfer_diagram, pair3, new_end);

	NFA_state_symbol_pair_type *pair4 = NFA_state_symbol_pair_create(array_list_get(right_nfa->end, 0), epsilon4);
	hash_table_insert(new_nfa->transfer_diagram, pair4, new_end);

	merge_nfas(nfas, new_nfa);

	return new_nfa;
}

static NFA_type *
merge_NFA_for_repeat(array_list_type *nfas)
{
	// because "repaat (*)" is a unary operator
	assert(nfas->length == 1);

	//left sub-node must have only one "end" state
	NFA_type *left_nfa = TYPE_CAST(array_list_get(nfas, 0), NFA_type *);
	assert(left_nfa->end->length == 1);

	// one big new NFA, to subsititude the "left" NFA.
	NFA_type *new_nfa = NFA_create();

	//
	// new "start" and "end" states for the big NFA:
	//

	NFA_state_type *new_start = NFA_state_create();
	array_list_append(new_nfa->states, new_start);
	new_nfa->start = new_start;

	NFA_state_type *new_end = NFA_state_create();
	array_list_append(new_nfa->states, new_end);
	array_list_append(new_nfa->end, new_end);

	//
	// update transfer diagram:
	//

	string_buffer epsilon1 = string_buffer_create();
	string_buffer_append(&epsilon1, "EPSILON");

	string_buffer epsilon2 = string_buffer_create();
	string_buffer_append(&epsilon2, "EPSILON");

	string_buffer epsilon3 = string_buffer_create();
	string_buffer_append(&epsilon3, "EPSILON");

	string_buffer epsilon4 = string_buffer_create();
	string_buffer_append(&epsilon4, "EPSILON");

	NFA_state_symbol_pair_type *pair1 = NFA_state_symbol_pair_create(new_start, epsilon1);
	hash_table_insert(new_nfa->transfer_diagram, pair1, left_nfa->start);

	NFA_state_symbol_pair_type *pair2 = NFA_state_symbol_pair_create(new_start, epsilon2);
	hash_table_insert(new_nfa->transfer_diagram, pair2, new_end);

	NFA_state_symbol_pair_type *pair3 = NFA_state_symbol_pair_create(array_list_get(left_nfa->end, 0), epsilon3);
	hash_table_insert(new_nfa->transfer_diagram, pair3, new_end);

	NFA_state_symbol_pair_type *pair4 = NFA_state_symbol_pair_create(array_list_get(left_nfa->end, 0), epsilon4);
	hash_table_insert(new_nfa->transfer_diagram, pair4, left_nfa->start);

	merge_nfas(nfas, new_nfa);

	return new_nfa;
}

static NFA_type *
merge_NFA_for_unit(array_list_type *nfas)
{
	// because "()" is a unary operator
	assert(nfas->length == 1);

	NFA_type *nfa = array_list_get(nfas, 0);

	return nfa;
}

NFA_type *
build_NFA_from_node(Ast_node_type *node)
{
	assert(node);

	NFA_type *new_nfa = NULL;

	if (node->is_operator_node) {
		//
		// operator node:
		//
		array_list_type *nfas = array_list_create();

		int i;
		for (i = 0; i < OPERATOR_NODE(node)->operand_nodes->length; i++) {
			// build NFA for each sub-node.
			Ast_node_type *sub_node = (Ast_node_type *)array_list_get(OPERATOR_NODE(node)->operand_nodes, i);

			NFA_type *sub_nfa = build_NFA_from_node(sub_node);

			array_list_append(nfas, sub_nfa);
		}

		// build NFA for this node, by merge sub-nodes' NFAs.
		production_token_type operator_type = OPERATOR_NODE(node)->operator->type;
		switch (operator_type) {
			case CONCAT:
				new_nfa = merge_NFA_for_concat(nfas);
				break;
			case VERTICAL_BAR:
				new_nfa = merge_NFA_for_alter(nfas);
				break;
			case STAR:
				new_nfa = merge_NFA_for_repeat(nfas);
				break;
			case LEFT_PARENTHESIS:
				new_nfa = merge_NFA_for_unit(nfas);
				break;
			default:
				LOG(NFA_LOG_ENABLE, "Oops! Unknown operator type: %d", operator_type);
				break;
		}

		array_list_destroy(nfas, NULL);
	}
	else {
		//
		// operand node:
		//

		new_nfa = NFA_create();

		char *desc = OPERAND_NODE(node)->desc;
		assert(strlen(desc) == 1);

		NFA_state_type *start = NFA_state_create();
		start->id = 0;
		NFA_state_type *end = NFA_state_create();
		end->id = 1;

		new_nfa->start = start;
		array_list_append(new_nfa->end, end);

		array_list_append(new_nfa->states, start);
		array_list_append(new_nfa->states, end);

		string_buffer symbol = string_buffer_create();
		string_buffer_append(&symbol, desc);

		NFA_state_symbol_pair_type *key = NFA_state_symbol_pair_create(start, symbol);
		hash_table_insert(new_nfa->transfer_diagram, key, end);

		//LOG(TRUE, "new_nfa: %s", get_NFA_debug_str(new_nfa));

	}

	char *debug_str = get_NFA_debug_str(new_nfa);
	LOG(NFA_LOG_ENABLE, "one big NFA: %s", debug_str);
	free(debug_str);

	return new_nfa;
}

static NFA_type *
NFA_from_AST(Ast_type *ast)
{
	if (ast == NULL) return NULL;

	NFA_type *nfa = NULL;

	// postorder tree tranversal.
	if (ast->root != NULL) {
		nfa = build_NFA_from_node(ast->root);
	}

	return nfa;
}

NFA_type *NFA_from_str(char *str)
{
	Ast_type *ast = AST_from_str(str);

	char *ast_debug_str = get_Ast_debug_str(ast, NULL);
	LOG(NFA_LOG_ENABLE, "ast: %s", ast_debug_str);
	free(ast_debug_str);

	NFA_type *nfa = NFA_from_AST(ast);

	Ast_destroy(ast, NULL);

	return nfa;
}

int
NFA_state_symbol_pair_hash(void *key)
{
	assert(key);

	NFA_state_symbol_pair_type *k = (NFA_state_symbol_pair_type *)key;
	LOG(TRUE, "id: %d, symbol: %s, hash: %d", k->state->id, k->symbol, k->state->id * 10 + *(k->symbol));
	return k->state->id * 10 + *(k->symbol);
}

bool
NFA_state_symbol_pair_compartor(void *one, void *another, va_list arg_list)
{
	NFA_state_symbol_pair_type *a = one;
	NFA_state_symbol_pair_type *b = another;

	assert(a && b);

	return a->state == b->state && !strcmp(a->symbol, b->symbol);

}

char *
get_NFA_debug_str(NFA_type *self)
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
				NFA_state_symbol_pair_type *key = ele->key;
				NFA_state_type *source = key->state;
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
				NFA_state_type *target = ele->value;
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

	assert(self->end->length);
	for (i = 0; i < self->end->length; ++i) {
		NFA_state_type *end_state = array_list_get(self->end, i);

		char *end_id_str = my_itoa(end_state->id);
		string_buffer_append(&debug_str, end_id_str);
		free(end_id_str);

		string_buffer_append(&debug_str, " [shape=rectangle];");
	}
	string_buffer_append(&debug_str, " }");
	return debug_str;
}

char *
NFA_state_debug_str(NFA_state_type *state, va_list arg_list)
{
	return int_to_str(&(state->id), NULL);
}
