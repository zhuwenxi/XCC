from .ast import *
from .context_free_grammar import *
from ..util import Stack
from ..config.config import *

import copy



class LR0Parser(object):
    def __init__(self, grammar):
        self.grammar = grammar
        self.states = []

        self.construct_canonical_collection()

    def parse(self, token_seq):
        print('LR0Parser')
        self.closure()

    def construct_canonical_collection(self):
        # Set0: closure([S' -> DOT S])
        prod = copy.deepcopy(self.grammar.productions[0])
        prod.bodies[0].insert(0, Symbol.DOT_SYMBOL)
        state0 = LR0Set([prod], self.grammar)
        self.states = {state0}

        for state in self.states:
            for prod in state.productions:
                for body in prod.bodies:
                    cand = state.symbol_after_dot(body)
                    if cand is not None:
                        dest_state = self.goto(state, cand)
                        if dest_state != None:
                            self.states.add(dest_state)

        self.states = list(self.states)

    def goto(self, state, symbol):
        return None

class LR0Set(object):
    def __init__(self, productions=None, grammar=None):
        self.productions = productions
        self.grammar = grammar

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