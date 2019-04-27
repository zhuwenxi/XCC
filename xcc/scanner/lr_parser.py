from .ast import *
from .context_free_grammar import *
from ..util import Stack
from ..config.config import *




class LRParser(object):
	def __init__(self, grammar):
		self.grammar = grammar

	def parse(self, token_seq):
		pass