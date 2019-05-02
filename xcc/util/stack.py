class Stack(object):
	def __init__(self):
		self.impl = []

	def push(self, elem):
		self.impl.append(elem)

	def pop(self):
		return self.impl.pop()

	def pop_n(self, n=1):
		ret = []
		for _ in range(n):
			ret.append(self.impl.pop())

		return ret

	def empty(self):
		return len(self.impl) == 0

	def peek(self):
		return self.impl[-1]