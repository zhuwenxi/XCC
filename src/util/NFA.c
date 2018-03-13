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
	hash_table_destroy(self->transfer_diagram, NULL);

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
	NFA_state_destroy(self->state, NULL);
	char_deconstructor(self->symbol, NULL);
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
			array_list_append(big_nfa, state);
		}

		// update transfer diagram:
		hash_table_type *sub_trans_diag = nfa->transfer_diagram;
		int bucket_index;
		for (bucket_index = 0; bucket_index < sub_trans_diag->buckets->length; bucket_index++) {
			array_list_node_type *bucket = array_list_get(sub_trans_diag->buckets, bucket_index);
			if (bucket != NULL) {
				linked_list_type *ll = bucket->data;
				linked_list_node_type *ll_node = ll->head;

				while (ll_node) {
					hash_table_element_type *ele = ll_node->data;
					NFA_state_symbol_pair_type *key = ele->key;
					NFA_state_type *value = ele->value;

					// update in big NFA's transfer diagram:
					hash_table_insert(big_nfa, key, value);
					
					ll_node = ll_node->next;
				}
			}
		}

		// destroy the sub-NFA. (have to be partial destroyed, since some objects are still refered by the big NFA)
		nfa->start = NULL;
		array_list_destroy(nfa->end, NULL);
		array_list_destroy(nfa->states, NULL);
		hash_table_destroy(nfa->transfer_diagram, NULL);
	}
}

static NFA_type *
merge_NFA_for_concat(array_list_type *nfas)
{
	// because "concat" is a binary operator
	assert(nfas->length == 2);

	// the NFA of left sub-node must have only one "end" state
	NFA_type *left_nfa = TYPE_CAST(array_list_get(nfas, 0), NFA_type *);
	LOG(TRUE, "left_nfa->end->length: %d", left_nfa->end->length);
	assert(left_nfa->end->length == 1);

	// one big new NFA, to subsititude the "left" and "right" NFAs.
	NFA_type *new_nfa = NFA_create();

	merge_nfas(nfas, new_nfa);


}

NFA_type *
build_NFA_from_node(NFA_type *nfa, Ast_node_type *node)
{
	assert(nfa && node);

	NFA_type *new_nfa = NFA_create();

	if (node->is_operator_node) {
		//
		// operator node:
		//
		array_list_type *nfas = array_list_create();

		int i;
		for (i = 0; i < OPERATOR_NODE(node)->operand_nodes->length; i++) {
			// build NFA for each sub-node.
			Ast_node_type *sub_node = (Ast_node_type *)array_list_get(OPERATOR_NODE(node)->operand_nodes, i);

			NFA_type *sub_nfa = build_NFA_from_node(nfa, sub_node);

			array_list_append(nfas, sub_nfa);
		}

		// build NFA for this node, by merge sub-nodes' NFAs.
		production_token_type operator_type = OPERATOR_NODE(node)->operator->type;
		switch (operator_type) {
			case CONCAT:
				merge_NFA_for_concat(nfas);
				break;
			case REPEAT:
				break;
			default:
				break;
		}
	}
	else {
		//
		// operand node:
		//

		char *desc = OPERAND_NODE(node)->desc;
		assert(strlen(desc) == 1);

		NFA_state_type *start = NFA_state_create();
		NFA_state_type *end = NFA_state_create();

		new_nfa->start = start;
		array_list_append(new_nfa->end, end);

		char *symbol = char_create(desc[0]);
		NFA_state_symbol_pair_type *key = NFA_state_symbol_pair_create(start, symbol);
		hash_table_insert(new_nfa->transfer_diagram, key, end);

	}

	return new_nfa;
}

static NFA_type *
NFA_from_AST(Ast_type *ast)
{
	if (ast == NULL) return NULL;

	NFA_type *nfa = NFA_create();

	// postorder tree tranversal.
	if (ast->root != NULL) {
		build_NFA_from_node(nfa, ast->root);
	}

	return nfa;
}

NFA_type *NFA_from_str(char *str)
{
	Ast_type *ast = AST_from_str(str);

	LOG(NFA_LOG_ENABLE, "ast: %s", get_Ast_debug_str(ast, NULL));

	NFA_type *nfa = NFA_from_AST(ast);

	return NULL;
}

int NFA_state_symbol_pair_hash(void *key)
{
	assert(key);

	NFA_state_symbol_pair_type *k = (NFA_state_symbol_pair_type *)key;

	return k->state->id % 10 + k->symbol;
}
