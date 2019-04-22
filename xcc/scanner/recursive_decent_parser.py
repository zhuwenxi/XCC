from .ast import *
from .context_free_grammar import *

class RecusiveDecentParser(object):
	def __init__(self, grammar=None):
		self.grammar = grammar

	def _match(self, symbol, token_seq, root_node):
		assert root_node is not None

		if symbol.is_terminal:
			if symbol.type == Symbol.EPSILON:
				new_node = Node(Symbol('EPSILON', is_terminal=True, symbol_type=Symbol.EPSILON))
				root_node.add_child(new_node)
				
				return True
			elif len(token_seq) == 0:
				return False
			elif token_seq[0].type == symbol:
				new_node = Node(symbol)
				root_node.add_child(new_node)
				token_seq.pop(0)

				return True
			else:
				return False
		else:
			# Candidate productions: head == symbol
			db_log(LOG_RECUISIVE_DECENT_PARSER, '\ntry to match {}'.format(symbol))
			cand_prods = [prod for prod in self.grammar.productions if prod.head == symbol]

			for prod in cand_prods:
				new_root_node = Node(symbol, children=[], is_leaf=False)
				db_log(LOG_RECUISIVE_DECENT_PARSER, 'create a new node:', new_root_node)
				for body in prod.bodies:
					token_seq_copy = token_seq.copy()
					body_match = True

					for body_symbol in body:
						body_match = self._match(body_symbol, token_seq_copy, new_root_node)
						if not body_match:
							break

					if body_match:
						# Body is match, update token_seq
						token_seq[:] = token_seq_copy
						root_node.add_child(new_root_node)
						return True
					else:
						db_log(LOG_RECUISIVE_DECENT_PARSER, 'not match {}'.format(body))
						# root_node.children = []

			return False

	def parse(self, token_seq):
		# "Goal" symbol
		target_symbol = self.grammar.productions[0].head
		true_target = self.grammar.productions[0].bodies[0][0]
		root = Node(target_symbol)

		status = self._match(true_target, token_seq, root)

		if status:
			ast = Ast(root)
		else:
			ast = None
		
		return ast