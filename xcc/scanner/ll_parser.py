from .ast import *
from .context_free_grammar import *
from ..util import Stack
from ..config.config import *




class LLParser(object):
    def __init__(self, grammar):
        self.grammar = grammar

        # Table[A, w] = prod
        self.parsing_table = {}
        self.construct_parsing_table()

    def parse(self, token_seq):
        stack = Stack()
        node_stack = Stack()

        # Push EOF to stack 
        stack.push(Symbol.EOF_SYMBOL)
        node_stack.push('dummy node')

        # Push "Goal" to stack
        target_symbol = self.grammar.productions[0].head
        stack.push(target_symbol)

        root = Node(target_symbol)
        node_stack.push(root)

        token_idx = 0

        

        while not stack.empty():
            A = stack.peek()
            assert isinstance(A, Symbol)

            if token_idx >= len(token_seq):
                raise Exception('Parsing failed! Token exhausted before parsing finished.')

            current_token = token_seq[token_idx]
            token_type = current_token.type

            if A.is_terminal:
                if current_token.type == A:
                    db_log(LOG_LL_PARSE, "Match terminal: {}".format(A))
                    stack.pop()

                    leaf_node = node_stack.pop()
                    try:
                        leaf_node.data = current_token
                    except:
                        print('leaf_node:', leaf_node)
                    
                    token_idx += 1
                else:
                    raise Exception('terminal mismatch, {} vs. {}'.format(current_token.type, A))
            else:
                body = self.transfer(A, token_type)
                if body != None:
                    db_log(LOG_LL_PARSE, "Match body: {}".format(body))
                    # Pop A
                    stack.pop()
                    node = node_stack.pop()
                    # Push bk, bk-1...b2, b1
                    new_nodes = []
                    for s in list(reversed(body)):
                        new_node = Node(s)
                        new_nodes.append(new_node)

                        if s != Symbol.EPSILON_SYMBOL:
                            stack.push(s)
                            node_stack.push(new_node)
                        
                    for new_node in list(reversed(new_nodes)):
                        node.add_child(new_node)
                else:
                    raise Exception('No entry for [{}, {}]'.format(A, token_type))

        if token_idx != len(token_seq):
            raise Exception('Parsing failed! Still tokens left after parsing is finished.')

        return Ast(root)

    def transfer(self, head, symbol):
        key = (head, symbol)

        if key in self.parsing_table:
            return self.parsing_table[key]
        else:
            return None

    def construct_parsing_table(self):
        if not self.grammar.check_backtrace_free():
            raise Exception('Not a backtrace-free grammar')

        for prod in self.grammar.productions:
            for body in prod.bodies:
                first_plus_key = (prod.head, tuple(body))
                first_plus_set = self.grammar.first_plus[first_plus_key]

                for symbol in first_plus_set:
                    if symbol == Symbol.EPSILON_SYMBOL:
                        continue

                    parsing_table_key = (prod.head, symbol)

                    if parsing_table_key in self.parsing_table:
                        raise Exception('Oops, parsing table key conflict!')

                    self.parsing_table[parsing_table_key] = body