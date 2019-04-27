class Stack(object):
	def __init__(self):
		self.impl = []

	def push(self, elem):
		self.impl.append(elem)

	def pop(self):
		return self.impl.pop()

	def empty(self):
		return len(self.impl) == 0

	def peep(self):
		return self.impl[-1]