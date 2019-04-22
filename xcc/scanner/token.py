from .context_free_grammar import Symbol

class Token(object):
	debug = False

	def __init__(self, text=None, token_type=None):
		self.text = text

		if token_type is not None and not isinstance(token_type, Symbol):
			raise Expection('{} is not an instance of Symbol'.format(token_type))

		self.type = token_type

	def __str__(self):
		return '"' + self.text + '"' if Token.debug else '("' + self.text + '", ' + str(self.type) + ')'

	def __repr__(self):
		return self.__str__()  