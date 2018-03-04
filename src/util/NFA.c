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

	self->transfer_diagram = hash_table_create(NFA_state_hash);
	self->states = array_list_create();

	return self;
}

bool NFA_deconstructor(NFA_type *self)
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

NFA_type *
build_NFA_from_node(NFA_type *nfa, Ast_node_type *node)
{
	assert(nfa && node);

	if (node->is_operator_node) {
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
				break;
			case REPEAT:
				break;
			default:
				break;
		}
	}
}

static NFA_type *
NFA_from_AST(Ast_type *ast)
{
	if (ast == NULL) return NULL;

	NFA_type *nfa = NFA_create();

	// postorder tree tranversal.
	if (ast->root != NULL) {
		build_NFA_from_node(nfa, ast);
	}
}

NFA_type *NFA_from_str(char *str)
{
	Ast_type *ast = AST_from_str(str);

	LOG(NFA_LOG_ENABLE, "ast: %s", get_Ast_debug_str(ast, NULL));

	return NULL;
}

int NFA_state_hash(void *state)
{
	return TYPE_CAST(state, NFA_state_type *)->id;
}
