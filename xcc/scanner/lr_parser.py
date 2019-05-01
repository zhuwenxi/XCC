from .ast import *
from .context_free_grammar import *
from ..util import Stack
from ..config.config import *

import copy



class LR0Parser(object):
    def __init__(self, grammar):
        self.grammar = grammar
        self.states = []
        self.goto_table = {}

        self.construct_canonical_collection()

    def parse(self, token_seq):
        print('LR0Parser')

    def construct_canonical_collection(self):
        # Set0: closure([S' -> DOT S])
        prod = copy.deepcopy(self.grammar.productions[0])
        prod.bodies[0].insert(0, Symbol.DOT_SYMBOL)
        state0 = LR0Set([prod], self.grammar)
        self.states = [state0]

        has_new_states = True
        while has_new_states:
            has_new_states = False

            for i, state in enumerate(self.states):
                state = self.states[i]
                for prod in state.productions:
                    for body in prod.bodies:
                        cand = state.symbol_after_dot(body)
                        if cand is not None:
                            db_log(LOG_LR_0_PARSE, "source: {}".format(state))
                            db_log(LOG_LR_0_PARSE, "symbol: {}".format(cand))
                            dest_state = self.goto(state, cand)
                            db_log(LOG_LR_0_PARSE, "target: {}".format(dest_state))
                            if dest_state != None and dest_state not in self.states:
                                has_new_states = True
                                self.states.append(dest_state)

        self._set_states_id()

    def goto(self, state, symbol):
        assert isinstance(state, LR0Set) and isinstance(symbol, Symbol)

        key = (state, symbol)
        if key in self.goto_table:
            return self.goto_table[key]
        # print('goto(,{})'.format(symbol))
        target_prods = []
        for prod in state.productions:
            prod_copy = copy.deepcopy(prod)
            # print('======================== prod_copy:', prod_copy)

            bodies_to_remove = []
            for body in prod_copy.bodies:
                symbol_after_dot = state.symbol_after_dot(body)
                # print('body: {} -> {}'.format(prod.head, body))
                # print('symbol_after_dot:', symbol_after_dot)
                if symbol_after_dot != symbol:
                    bodies_to_remove.append(body)
            
            # print('remove list:', bodies_to_remove)
            for b in bodies_to_remove:
                # print('remove body:', b)
                prod_copy.bodies.remove(b)

            if len(prod_copy.bodies) > 0:
                # print('symbol: {}, prod_copy:{}'.format(symbol, prod_copy))
                self._dot_move_forward(prod_copy)
                target_prods.append(prod_copy)

        # print('!!!!!!!!!!!!!!!!!!!!!!!target_prods:', target_prods)
        if len(target_prods) == 0:
            self.goto_table[key] = None
        else:
            target_state = LR0Set(target_prods, self.grammar)
            if target_state in self.states:
                target_state = self.states[self.states.index(target_state)]
            self.goto_table[key] = target_state

        return self.goto_table[key]

    def _dot_move_forward(self, prod):
        assert isinstance(prod, Production)

        for body in prod.bodies:
            index_of_dot = body.index(Symbol.DOT_SYMBOL)
            index_of_symbol_after_dot = index_of_dot + 1

            if index_of_symbol_after_dot >= len(body):
                raise Exception("DOT is already at the tail of the body: {}".format(body))
            # Swap DOT with the symbol follows immediately after it.
            temp = body[index_of_dot]
            body[index_of_dot] = body[index_of_symbol_after_dot]
            body[index_of_symbol_after_dot] = temp

    def _set_states_id(self):
        for i, state in enumerate(self.states):
            state.id = i

    def __str__(self):
        ret_str = ''
        for i, state in enumerate(self.states):
            ret_str += '\n============================ state {} ============================\n'.format(i)

            for prod in state.productions:
                for body in prod.bodies:
                    ret_str += '{} -> {}'.format(prod.head, body) + '\n'

        ret_str += '============================ GOTO table ============================\n'
        ret_str += self.goto_table_str()

        return ret_str

    def __repr__(self):
        return str(self)

    def goto_table_str(self):
        ret_str = ''

        ret_str += '\ndigraph\n{\n'

        for key in self.goto_table:
            target = self.goto_table[key]
            if target is not None:
                state, symbol = key
                ret_str += '{} -> {} [label="{}"];\n'.format(state.id, target.id, symbol)
        
        ret_str += '}'

        return ret_str

class LR0Set(object):
    def __init__(self, productions=None, grammar=None):
        self.productions = productions
        self.grammar = grammar
        self.id = -1

        self.closure()

    def closure(self):
        work_queue = copy.deepcopy(self.productions)
        while len(work_queue) > 0:
            prod = work_queue.pop()
            for body in prod.bodies:
                symbol_after_dot = self.symbol_after_dot(body)
                if symbol_after_dot and not symbol_after_dot.is_terminal:
                    prods = self._prod_start_with(symbol_after_dot)
                    self._add_dot_to_body_front(prods)
                    
                    for new_prod in prods:
                        if new_prod in self.productions:
                            continue
                        # Add these productions to set
                        self.productions.append(new_prod)
                        # Add these productions to work queue
                        work_queue.append(new_prod)
                        
    def symbol_after_dot(self, body):
        index_of_dot = body.index(Symbol.DOT_SYMBOL)
        index_of_symbol_after_dot = index_of_dot + 1

        return body[index_of_symbol_after_dot] if index_of_symbol_after_dot < len(body) else None

    def _prod_start_with(self, symbol):
        assert not symbol.is_terminal

        ret = []

        for prod in self.grammar.productions:
            if prod.head == symbol:
                ret.append(copy.deepcopy(prod))

        return ret

    def _add_dot_to_body_front(self, prods):
        assert prods is not None

        for prod in prods:
            for body in prod.bodies:
                body.insert(0, Symbol.DOT_SYMBOL)

    def __str__(self):
        return str(self.productions)

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        return str(self) == str(other)

    def __hash__(self):
        return hash(tuple(self.productions))

class LR1Parser(LR0Parser):
    pass