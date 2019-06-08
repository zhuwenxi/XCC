from .context_free_grammar import Symbol




class Ast(object):
	def __init__(self, root=None):
		self.root = root

		self.set_all_nodes_id()

	def __str__(self):
		ret_str = '\ndigraph\n{\n'
		node_queue = []

		if self.root:
			node_queue.append(self.root)

		while len(node_queue) > 0:
			node = node_queue.pop(0)

			if not node.children:
				continue

			for child in node.children:
				ret_str += '{}'.format(node.graphviz_str()) + ' -> ' + '{}'.format(child.graphviz_str()) + ';\n'
				node_queue.append(child)

		ret_str += '}\n'

		return ret_str

	def set_all_nodes_id(self):
		node_id = 0

		node_queue = [self.root]

		while len(node_queue) > 0:
			node = node_queue.pop(0)

			node.id = node_id
			node_id += 1

			for child in node.children:
				node_queue.append(child)

class Node(object):
	debug = False

	def __init__(self, data=None, children=None, is_leaf=False, node_id=-1):
		# if not isinstance(data, Symbol):
		# 	raise TypeError('"data" must has a type of Symbol, but got {} instead.'.format(type(data)))
		self.data = data
		if children is None:
			self.children = []
		else:
			self.children = children
		self.is_leaf = is_leaf
		self.id = node_id

	def __str__(self):
		return str(self.data) if not Node.debug else str(self.data) + ' ({})'.format(id(self))

	def __repr__(self):
		return str(self)

	def add_child(self, child):
		if not isinstance(child, Node):
			raise TypeError('"child" must be a type of Node, but got {} instead.'.format(type(child)))
		self.children.append(child)

	def graphviz_str(self):
		return '{' + str(self.id) + ' [label="' + str(self) +'"]}'
		# return '{' + str(hex(id(self))) + ' [label="' + str(self) +'"]}'