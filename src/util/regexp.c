#include "regexp.h"
#include "string_buffer.h"



char *token_desc_table[] =
{
#define PRODUCTION_TOKEN(code, name) name,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#include "expression_grammar.def"
#undef PRODUCTION_TOKEN
	"PRODUCTION_TOKEN_LIMIT"
};

regexp_type *
regexp_create(char *text)
{
	regexp_type *self = (regexp_type *)malloc(sizeof(regexp_type));
	self->text = text;

	NFA_type *nfa = NFA_from_str(text);
	DFA_type *dfa = NFA_to_DFA(nfa);
	self->DFA = dfa;

	return self;
}

bool
regexp_deconstructor(regexp_type *self, va_list arg_list)
{
	DFA_destroy(self->DFA, NULL);

	free(self);
	
	return TRUE;
}

char *
_pattern_preprocess(char *pattern)
{
	// Phase 1:
	// Expand "[a-z]" to "(a|b|c|...|z)";
	string_buffer processed_pattern = string_buffer_create();
	string_buffer_append(&processed_pattern, pattern);

	while (string_buffer_search_char(processed_pattern, '[') >= 0)
	{
		int start = string_buffer_search_char(processed_pattern, '[');
		if (start == -1) break;

		int end = string_buffer_search_char(processed_pattern, ']') + 1;
		assert(end > 0);

		LOG(REGEXP_PREPROCESS_LOG_ENABLE, "pattern before: %s", processed_pattern);

		string_buffer before_bracket = string_buffer_substr(processed_pattern, 0, start);
		LOG(REGEXP_PREPROCESS_LOG_ENABLE, "before_bracket: %s", before_bracket);
		string_buffer after_bracket = string_buffer_substr(processed_pattern, end, strlen(processed_pattern));
		LOG(REGEXP_PREPROCESS_LOG_ENABLE, "after_bracket: %s", after_bracket);

		// Process the sub-string part "[...]":
		string_buffer substr = string_buffer_substr(processed_pattern, start + 1, end - 1);
		while (string_buffer_search_char(substr, '-') >= 0)
		{
			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "substr before this iteration: %s", substr);

			int dash_idx = string_buffer_search_char(substr, '-');
			if (dash_idx < 0) break;

			char start_char = substr[dash_idx - 1];
			char end_char = substr[dash_idx + 1];

			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "start: %c, end: %c", start_char, end_char);

			string_buffer from_to = string_buffer_create();
			/*char tmp[2] = {'(', '\0'};
			string_buffer_append(&from_to, tmp);*/
			char c;
			for (c = start_char; c <= end_char; ++c)
			{
				char tmp[3] = {c, '\0', '\0'};
				//if (c != end_char) tmp[1] = '|';
				string_buffer_append(&from_to, tmp);
			}
			//char tmp2[2] = { ')', '\0' };
			//string_buffer_append(&from_to, tmp2);

			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "from_to: %s", from_to);

			string_buffer before = string_buffer_substr(substr, 0, dash_idx - 1);
			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "before: %s", before);
			string_buffer after = string_buffer_substr(substr, dash_idx + 2, strlen(substr));
			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "after: %s", after);

			string_buffer_destroy(substr);

			substr = string_buffer_create();
			string_buffer_append(&substr, before);
			string_buffer_append(&substr, from_to);
			string_buffer_append(&substr, after);

			string_buffer_destroy(before);
			string_buffer_destroy(from_to);
			string_buffer_destroy(after);

			LOG(REGEXP_PREPROCESS_LOG_ENABLE, "substr after this iteration: %s", substr);
		}

		string_buffer_destroy(processed_pattern);
		
		processed_pattern = string_buffer_create();
		string_buffer_append(&processed_pattern, before_bracket);
	
		char tmp[2] = { '(', '\0' };
		string_buffer_append(&processed_pattern, tmp);
		//string_buffer_append(&processed_pattern, substr);
		int substr_idx;
		for (substr_idx = 0; substr_idx < strlen(substr); ++substr_idx)
		{
			char tmp[3] = { substr[substr_idx], '\0', '\0' };
			if (substr_idx != strlen(substr) - 1)
			{
				tmp[1] = '|';
			}
			string_buffer_append(&processed_pattern, tmp);
		}
		char tmp2[2] = {')', '\0'};
		string_buffer_append(&processed_pattern, tmp2);

		string_buffer_append(&processed_pattern, after_bracket);
	}

	LOG(TRUE, "pattern after preprocess: %s", processed_pattern);
	
	// Phase 2:
	// 1. Replace "\\d" with "(0|1|2|3|4|5|6|7|8|9)";
	// 2. Replace "\\w" with "(a|b|c|...|z|A|B|C|D|...|Z)";
	// 3. Replace "a+" with "aa*";

	return processed_pattern;
}

regexp_return_group_type regexp_search(char *pattern, char *str)
{
	// Preprocessing:
	pattern = _pattern_preprocess(pattern);

	// regexp constructed from the "pattern" string.
	regexp_type *regexp = regexp_create(pattern);

	string_buffer ret = string_buffer_create();

	// We iterate every char in the "str", treat it
	// as the start of a matched pattern and try to
	// find the match.
	int str_idx;
	for (str_idx = 0; str_idx < strlen(str); ++str_idx)
	{
		int start_pos = str_idx;
		int end_pos = strlen(str) - 1;

		int cur_pos = start_pos;
		char cur_char = str[cur_pos];

		stack_type *state_stack = stack_create();

		LOG(REGEXP_LOG_ENABLE, "start char: %c", cur_char);

		// Record the ret string length.
		int ret_length = 1;

		DFA_type *dfa = regexp->DFA;
		assert(dfa);

		DFA_state_type *state = dfa->start;
		array_list_type *end_state_set = dfa->end;

		
		do {
			// "cur_char" is a char, make is a string (with exactly one element).
			string_buffer symbol = string_buffer_create();

			char tmp_str[2];
			tmp_str[0] = cur_char;
			tmp_str[1] = '\0';

			string_buffer_append(&symbol, tmp_str);

			DFA_state_symbol_pair_type key;
			key.state = state;
			key.symbol = symbol;

			char *source_state_debug_str = DFA_state_debug_str(key.state, NULL);
			LOG(REGEXP_LOG_ENABLE, "soruce state: %s", source_state_debug_str);
			free(source_state_debug_str);

			LOG(REGEXP_LOG_ENABLE, "symbol: %s", symbol);

			state = hash_table_search(dfa->transfer_diagram, &key, DFA_state_symbol_pair_compartor, NULL);

			if (state)
			{
				char *target_state_debug_str = DFA_state_debug_str(state, NULL);
				LOG(REGEXP_LOG_ENABLE, "target state: %s", target_state_debug_str);
				free(target_state_debug_str);

				++cur_pos;
				cur_char = str[cur_pos];
				++ret_length;

				stack_push(state_stack, state);
			}

			// De-construct the "symbol".
			free(symbol);
		} while (state != NULL && cur_pos <= end_pos);

		bool find_a_match = FALSE;
		// Roll back and try to find the matched state.
		while (!stack_empty(state_stack))
		{
			state = stack_pop(state_stack);
			--cur_pos;
			--ret_length;

			LOG(REGEXP_LOG_ENABLE, "start_pos: %d, ret_length: %d", start_pos, ret_length);
			
			if (array_list_search(end_state_set, state, DFA_state_compartor, NULL))
			{
				find_a_match = TRUE;
				break;
			}
		}

		stack_destroy(state_stack, NULL);

		if (find_a_match) 
		{
			LOG(REGEXP_LOG_ENABLE, "Find a match.");
			int ret_idx;
			for (ret_idx = start_pos; ret_idx < start_pos + ret_length; ++ret_idx)
			{
				char tmp[2] = { str[ret_idx], '\0' };
				string_buffer_append(&ret, tmp);
			}
			regexp_destroy(regexp, NULL);

			regexp_return_group_type group;
			group.start = start_pos;
			group.length = ret_length;
			group.str = ret;

			return group;
		}

		LOG(REGEXP_LOG_ENABLE, "Could not find a match.");
	}
	
	// Destroy the regexp and ret.
	regexp_destroy(regexp, NULL);
	free(ret);

	LOG(REGEXP_LOG_ENABLE, "Fail to search.")

	regexp_return_group_type group;
	group.start = -1;
	group.length = 0;
	group.str = NULL;

	return group;
}

production_token_type
regexp_grammar_get_token_type(char *str)
{
	char c = str[0];

	// Escaped char.
	if (c == '\\')
	{
		c = str[1];

		switch (c)
		{
			case '+':								  // Plus
				break;
			case '*':								  // Multiple
				break;
			default:
				LOG(TRUE, "Oops, unknown token: %c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", c);
				return -1;
		}

		return CHAR;
	}

	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'  // Alphabet
	 || c >= '0' && c <= '9'                          // Digit
	 || c == '_'									  // Underscore
	 || c == ' '									  // Whitespace
	 || c == '-'									  // Minus
	 || c == '/'									  // Divide
	 )
	{
		return CHAR;
	}
	else if (c == '*')
	{
		return STAR;
	}
	else if (c == '(')
	{
		return LEFT_PARENTHESIS;
	}
	else if (c == ')')
	{
		return RIGHT_PARENTHESIS;
	}
	else if (c == '|')
	{
		return VERTICAL_BAR;
	}
	else
	{
		LOG(TRUE, "Oops, unknown token: %c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", c);
		return -1;
	}
}

void
construct_ast_for_regexp_grammar(stack_type *ast_node_stack, production_type *prod_to_reduce)
{
	char *debug_str = production_debug_str(prod_to_reduce, token_desc_table);

	if (strcmp(debug_str, "Regexp -> Regexp | Concat Dot ") == 0)
	{
		Ast_node_type *node_Concat = stack_pop(ast_node_stack);

		// "|" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Regexp = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '|', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, VERTICAL_BAR);
		Ast_append_sub_node(node, node_Regexp);
		Ast_append_sub_node(node, node_Concat);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Regexp -> Concat Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Concat -> Concat Repeat Dot ") == 0)
	{
		Ast_node_type *node_Repeat = stack_pop(ast_node_stack);
		Ast_node_type *node_Concat = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '.', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, CONCAT);
		Ast_append_sub_node(node, node_Concat);
		Ast_append_sub_node(node, node_Repeat);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Concat -> Repeat Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Repeat -> Unit * Dot ") == 0)
	{
		// "*" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Unit = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '*', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, STAR);
		Ast_append_sub_node(node, node_Unit);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Repeat -> Unit Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Unit -> ( Regexp ) Dot ") == 0)
	{
		// "(" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Regexp = stack_pop(ast_node_stack);

		// ")" symbol:
		stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		string_buffer_append(&desc, "()");

		Ast_node_type *node = Ast_node_create(TRUE, desc, LEFT_PARENTHESIS);
		Ast_append_sub_node(node, node_Regexp);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Unit -> Char Dot ") == 0)
	{
		LR_automata_input_type *stack_elem = stack_pop(ast_node_stack);
		assert(stack_elem);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { stack_elem->c, '\0' };
		string_buffer_append(&desc, tmp);

		// create a operand node: "id"
		Ast_node_type *node = Ast_node_create(FALSE, desc, stack_elem->type);
		assert(node->is_operator_node == FALSE);
		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else
	{
		LOG(TRUE, "Unknown production:%s!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!, len: %d", debug_str, strlen(debug_str));
	}

	free(debug_str);
}
