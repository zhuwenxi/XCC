from ..util import type_check
from ..util.logging import db_log
from ..config.config import *

import copy

counter = 0

class Grammar(object):
	def __init__(self, grammar_dict):
		self.productions = []

		nonterminals = []
		for head in grammar_dict:
			bodies = grammar_dict[head]
			
			prod = Production(head, bodies)
			self.productions.append(prod)

			nonterminals.append(head)

		# Set "is_terminal" correctly
		for prod in self.productions:
			if prod.head.text in nonterminals:
				prod.head.is_terminal = False

			for body in prod.bodies:
				for symbol in body:
					if symbol.text in nonterminals:
						symbol.is_terminal = False

		# self.nonterminals.reverse()

		# FIRST set
		self.first = {}
		self.first_set_ready = False
		# FOLLOW set
		self.follow = {}
		self.follow_set_ready = False
		# FIRST+ set
		self.first_plus = {}
		self.first_plus_set_ready = False

	def nonterminals(self):
		ret = []
		for prod in self.productions:
			ret.append(prod.head)

		return ret

	# Find bodies of all productions expand Aj:
	def _find_body_expand_Aj(self, Aj):
		body_expand_Aj = []
		for prod in self.productions:
			if prod.head == Aj:
				for body in prod.bodies:
					body_expand_Aj.append(body)

		return body_expand_Aj

	# Return True is "production" is a reverse order production.
	def _replace_reversed_order_prod(self, Ai, Aj):
		db_log(LOG_CONTEXT_FREE_GRAMMAR, '_replace_reversed_order_prod(): {} -> {}'.format(Ai, Aj))
		for prod_idx, prod in enumerate(self.productions):
			if prod.head != Ai:
				continue

			bodies_to_replace = []
			for body_idx, body in enumerate(prod.bodies):
				symbol = body[0]
				if symbol != Aj:
					continue

				# Current production is Ai -> Aj..., expand Aj
				db_log(LOG_CONTEXT_FREE_GRAMMAR, 'find a candidate prod: {} -> {}'.format(prod.head, body))
				
				bodies_to_replace.append(body)
				global counter
				counter += 1
				if counter > 10:
					import sys
					sys.exit()

			for body in bodies_to_replace:
				bodies_expand_Aj = self._find_body_expand_Aj(Aj)

				# Expand Aj in the current production (Ai -> Aj...)
				rest_body = body[1:]
				prod.bodies.remove(body)

				for expand_body_idx, expand_body in enumerate(bodies_expand_Aj):
					new_body = expand_body + rest_body
					prod.bodies.insert(expand_body_idx, new_body)
		db_log(LOG_CONTEXT_FREE_GRAMMAR, 'after replace:\n', self)

	def eliminate_direct_left_recursion(self, Ai):
		# db_log(LOG_CONTEXT_FREE_GRAMMAR, "eliminate_direct_left_recursion:", Ai)
		for prod in self.productions:
			if prod.head != Ai:
				continue

			left_recursion_bodies = []
			right_recursion_bodies = []

			for body in prod.bodies:
				if body[0] == Ai:
					left_recursion_bodies.append(body)
				else:
					right_recursion_bodies.append(body)

			if len(left_recursion_bodies) > 0:
				Aj_bodies = []
				Aj_single_quote_bodies = []

				Aj_single_quote = Symbol(Ai.text + "'", is_terminal=False)

				for right_recursion_body in right_recursion_bodies:
					# Aj  -> beta Aj'
					Aj_bodies.append(right_recursion_body + [Aj_single_quote])
					# print('Aj_bodies:', Aj_bodies)
					for left_recursion_body in left_recursion_bodies:	
						# Aj' -> alpha Aj'
						Aj_single_quote_bodies.append(left_recursion_body[1:] + [Aj_single_quote])

				prod.bodies = Aj_bodies

				Aj_single_quote_prod = Production()
				Aj_single_quote_prod.head = Aj_single_quote
				Aj_single_quote_prod.bodies = Aj_single_quote_bodies + [[Symbol('EPSILON', symbol_type=Symbol.EPSILON)]]

				self.productions.insert(self.productions.index(prod) + 1, Aj_single_quote_prod)

	def eliminate_left_recursion(self):
		nonterminals = self.nonterminals()
		db_log(LOG_CONTEXT_FREE_GRAMMAR, 'A0, A1...An: {}\n'.format(nonterminals))

		nonterminals_num = len(nonterminals)

		for i in range(1, nonterminals_num):
			Ai = nonterminals[i]

			for j in range(0, i):
				Aj = nonterminals[j]

				self._replace_reversed_order_prod(Ai, Aj)

			# eliminate direct left recursion on Ai
			self.eliminate_direct_left_recursion(Ai)

		db_log(LOG_CONTEXT_FREE_GRAMMAR, "Right-recusive grammar:\n{}".format(self), flush=True)
	
	def get_all_symbols(self):
		symbols = []

		for prod in self.productions:
			if prod.head not in symbols:
				symbols.append(prod.head)

			for body in prod.bodies:
				for symbol in body:
					if symbol not in symbols:
						symbols.append(symbol)

		return symbols

	def compute_first_set(self):
		symbols = self.get_all_symbols()

		for s in symbols:
			self.first[s] = set()
			# Terminal, EPSILON and EOF
			if s.is_terminal or s is Symbol.EPSILON_SYMBOL or s is Symbol.EOF_SYMBOL:
				self.first[s].add(s)

		db_log(LOG_FIRST_SET, "====================== After initialization, first set: ======================\n{}\n".format(self.first))

		set_changed = True
		while set_changed:
			set_changed = False

			for prod in self.productions:
				for body in prod.bodies:
					body_len = len(body)

					first_body_0 = self.first[body[0]].copy()
					if Symbol.EPSILON_SYMBOL in first_body_0:
						first_body_0.remove(Symbol.EPSILON_SYMBOL)
					rhs = first_body_0

					for i, symbol in enumerate(body):
						if Symbol.EPSILON_SYMBOL not in self.first[body[i]]:
							break

						if i < body_len - 1:
							first_body_i_plus_1 = self.first[body[i + 1]].copy()
							if Symbol.EPSILON_SYMBOL in first_body_i_plus_1:
								first_body_i_plus_1.remove(Symbol.EPSILON_SYMBOL)
							rhs.update(first_body_i_plus_1)
					
					# if EPSILON in every first[body[i]], add EPSLION to rhs
					if i == body_len - 1 and Symbol.EPSILON_SYMBOL in self.first[body[body_len - 1]]:
						rhs.add(Symbol.EPSILON_SYMBOL)

					len_before_update = len(self.first[prod.head])
					self.first[prod.head].update(rhs)
					len_after_update = len(self.first[prod.head])

					if len_before_update != len_after_update:
						set_changed = True

		db_log(LOG_FIRST_SET, "====================== Done, first set: ======================")
		for symbol in self.first:
			l = list(self.first[symbol])
			l.sort()
			self.first[symbol] = l
			# db_log(LOG_FIRST_SET, "{}: {}".format(symbol, l))
		db_log(LOG_FIRST_SET, self.first)

		self.first_set_ready = True

	def compute_follow_set(self):
		if not self.first_set_ready:
			self.compute_first_set()

		symbols = self.get_all_symbols()

		# Initialize all non-terminals' follow set to empty set.
		for symbol in symbols:
			if not symbol.is_terminal:
				self.follow[symbol] = set()

		goal_symbol = self.productions[0].head

		self.follow[goal_symbol] = {Symbol.EOF_SYMBOL}

		follow_set_changed = True
		while follow_set_changed:
			follow_set_changed = False

			for prod in self.productions:
				for body in prod.bodies:
					trailer = self.follow[prod.head].copy()

					db_log(LOG_FOLLOW_SET, "=========================")

					db_log(LOG_FOLLOW_SET, "trailer init from {}: {}".format(prod.head, self.follow[prod.head]))
					db_log(LOG_FOLLOW_SET, "{} -> {}".format(prod.head, body))

					for symbol in list(reversed(body)):
						if not symbol.is_terminal:

							db_log(LOG_FOLLOW_SET, "trailer: {}".format(trailer))
							len_before_update = len(self.follow[symbol])
							self.follow[symbol].update(trailer)
							len_after_update = len(self.follow[symbol])
							
							db_log(LOG_FOLLOW_SET, "follow[{}]: {}".format(symbol, self.follow[symbol]))

							if len_before_update != len_after_update:
								follow_set_changed = True

							if Symbol.EPSILON_SYMBOL in self.first[symbol]:
								symbol_first_set = set(self.first[symbol].copy())
								symbol_first_set.remove(Symbol.EPSILON_SYMBOL)
								db_log(LOG_FOLLOW_SET, "before trailer update 1. {}".format(self.follow))
								trailer.update(symbol_first_set)
								db_log(LOG_FOLLOW_SET, "trailer update 1. {}".format(self.follow))

							else:
								trailer = set(self.first[symbol].copy())


						else:
							trailer = set(self.first[symbol].copy())



					db_log(LOG_FOLLOW_SET, "follow() after every body:{}\n".format(self.follow))


		for symbol in self.follow:
			l = list(self.follow[symbol])
			l.sort()
			self.follow[symbol] = l
		db_log(LOG_FOLLOW_SET, self.follow)

		self.follow_set_ready = True

	def compute_first_plus_set(self):
		if not self.follow_set_ready:
			self.compute_follow_set()

		for prod in self.productions:
			for body in prod.bodies:
				self.first_plus[(prod.head, tuple(body))] = set(self.first[body[0]].copy())

				epslion_in_first_set = []
				for symbol in body:
					epslion_in_first_set.append(Symbol.EPSILON_SYMBOL in self.first[symbol])

				if all(epslion_in_first_set):
					self.first_plus[(prod.head, tuple(body))].update(set(self.follow[prod.head]))

		for symbol in self.first_plus:
			l = list(self.first_plus[symbol])
			l.sort()
			self.first_plus[symbol] = l

		self.first_plus_set_ready = True

	def check_backtrace_free(self):
		if not self.first_plus_set_ready:
			self.compute_first_plus_set()

		for prod in self.productions:
			first_plus_set_all = set()
			all_set_len = 0
			for body in prod.bodies:
				key = (prod.head, tuple(body))
				first_plus_set = self.first_plus[key]

				all_set_len += len(first_plus_set)
				first_plus_set_all.update(first_plus_set)

			if len(first_plus_set_all) != all_set_len:
				return False
		
		return True

	# Deprecated
	def left_factoring(self):
		body_first_symbol_dict = {}
		for prod in self.productions:
			for body in prod.bodies:
				body_first_symbol = body[0]

				if body_first_symbol not in body_first_symbol_dict:
					body_first_symbol_dict = set()

				body_first_symbol_dict[body_first_symbol].add(body)

			for symbol, body in body_first_symbol_dict.items():
				pass

	def __str__(self):
		text = ''
		for prod in self.productions:
			text += str(prod) + '\n'

		return text


class Production(object):
	def __init__(self, head=None, bodies=None):
		if head:
			self.head = Symbol(head)

		self.bodies = []

		if bodies is None:
			return

		for body in bodies:
			symbol_body = []
			for symbol in body:
				symbol_body.append(Symbol(symbol))
			self.bodies.append(symbol_body)
				
	def __deepcopy__(self, mem):
		cpy = Production()
		cpy.head = copy.deepcopy(self.head)
		cpy.bodies = copy.deepcopy(self.bodies)
		return cpy

	def __str__(self):
		ret_str = str(self.head) + ' ->'

		for i, body in enumerate(self.bodies):
			for s in body:
				ret_str += ' ' + str(s)

			if i != len(self.bodies) - 1:
				ret_str += ' |'

		return ret_str

	def __repr__(self):
		return self.__str__()

	def __eq__(self, other):
		return self.head == other.head and self.bodies == other.bodies

	def __hash__(self):
		ret = hash(self.head)

		for body in self.bodies:
			for symbol in body:
				ret += hash(symbol)

		return ret


class Symbol(object):
	
	NORMAL = 0
	EPSILON = 1
	EOF = 2
	DOT = 3

	debug = False

	def __init__(self, text, is_terminal=True, symbol_type=NORMAL):
		self.text = text
		self.is_terminal = is_terminal
		self.type = symbol_type

	def __str__(self):
		return self.text if not Symbol.debug else self.text + '({})'.format(self.is_terminal)

	def __repr__(self):
		return self.__str__()

	def __eq__(self, other):
		if other is None or not isinstance(other, Symbol):
			return False
		else:
			return self.text == other.text and self.is_terminal == other.is_terminal

	def __lt__(self, other):
		return self.text < other.text

	def __hash__(self):
		return hash(self.text) + hash(self.is_terminal)

	def __deepcopy__(self, mem):
		return Symbol(self.text, self.is_terminal, self.type)

Symbol.EPSILON_SYMBOL = Symbol("EPSILON", is_terminal=True, symbol_type=Symbol.EPSILON)
Symbol.EOF_SYMBOL = Symbol("EOF", is_terminal=True, symbol_type=Symbol.EOF)
Symbol.DOT_SYMBOL = Symbol("DOT", is_terminal=True, symbol_type=Symbol.DOT)