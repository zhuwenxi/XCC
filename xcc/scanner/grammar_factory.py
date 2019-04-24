import os
import glob
import json

from .context_free_grammar import *


GRAMMAR_DIR = os.path.join(os.path.dirname(__file__), './json')

class GrammarFactory(object):
	_grammars = None

	def __init__(self):
		raise Exception('GrammarFactory can\'t be instantiated!!')

	@classmethod
	def get_grammar(cls, name):
		if cls._grammars is None:
			cls._grammars = cls.load_grammars()

			assert cls._grammars is not None

		if name not in cls._grammars:
			return None
		else:
			return cls._grammars[name]

	@classmethod
	def clear(cls):
		cls._grammars = None
		
	@classmethod
	def load_grammars(cls, grammar_dir=GRAMMAR_DIR):
		grammar_json_files = glob.glob(os.path.join(grammar_dir, '*.json'))

		_grammars = {}

		for grammar_json_file in grammar_json_files:
			filename = os.path.splitext(os.path.basename(grammar_json_file))[0]
			if filename in _grammars:
				raise Exception('{} conflicts with existing grammar'.format(grammar_json_file))

			with open(grammar_json_file) as f:
				grammar_json = json.load(f)
				grammar = Grammar(grammar_json)
				_grammars[filename] = grammar

		return _grammars