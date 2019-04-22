class TypeError(Exception):
	def __init__(self, message):
		self.message = message

def check(obj, cls):
	if not isinstance(obj, cls):
		raise TypeError(obj + ' is not a valid ' + cls + ' instance.')