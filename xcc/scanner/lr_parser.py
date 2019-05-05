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
        self.action_table = {}

        self.construct_parsing_tables()

    def parse(self, token_seq):
        # Push s0 into stack.
        state_stack = Stack()
        state_stack.push(self.states[0])

        # Node-stack contains AST tree nodes.
        node_stack = Stack()

        token_index = 0

        while True:
            state = state_stack.peek()
            curr_token = token_seq[token_index]
            symbol = curr_token.type
            key = (state, symbol)

            db_log(LOG_LR_0_PARSE, 'state:', state.id)
            db_log(LOG_LR_0_PARSE, 'token:', curr_token)
            action = self.action_table[key]
            db_log(LOG_LR_0_PARSE, 'action:', action)

            if action.action_type == Action.SHIFT:
                state_stack.push(action.shift_state)

                leaf_node = Node(curr_token, is_leaf=True)
                node_stack.push(leaf_node)

                token_index += 1
            elif action.action_type == Action.REDUCE:
                # "-1" means we don't count DOT in.
                len_states_to_pop = len(action.reduce_prod.bodies[0]) - 1
                state_stack.pop_n(len_states_to_pop)

                children = list(reversed(node_stack.pop_n(len_states_to_pop)))
                interior_node = Node(action.reduce_prod.head, children=children)
                node_stack.push(interior_node)

                state = state_stack.peek()
                symbol = action.reduce_prod.head
                key = (state, symbol)
                target = self.goto_table[key]
                state_stack.push(target)
            elif action.action_type == Action.ACCEPT:
                break
            else:
                # raise Exception('Meet a error action')
                break

        if action.action_type == Action.ACCEPT:
            assert len(node_stack.impl) == 1

            return Ast(root=node_stack.pop())

        return None


    def construct_parsing_tables(self):
        self.construct_canonical_collection()
        self.construct_action_table()

    def initial_state(self):
        # Set0: closure([S' -> DOT S])
        prod = copy.deepcopy(self.grammar.productions[0])
        prod.bodies[0].insert(0, Symbol.DOT_SYMBOL)
        
        return LR0Set([LR0Item(prod)], self.grammar)

    def construct_canonical_collection(self):
        state0 = self.initial_state()
        self.states = [state0]

        has_new_states = True
        while has_new_states:
            has_new_states = False

            for i, state in enumerate(self.states):
                state = self.states[i]
                for item in state.items:
                    for body in item.production.bodies:
                        cand = state.symbol_after_dot(body)
                        if cand is not None:
                            dest_state = self.goto(state, cand)
                            if dest_state != None and dest_state not in self.states:
                                has_new_states = True
                                self.states.append(dest_state)

        self._set_states_id()

    def construct_action_table(self):
        goal_symbol = self.grammar.productions[0].head
        eof_symbol = Symbol.EOF_SYMBOL

        for state in self.states:
            for item in state.items:
                prod = item.production
                for body in prod.bodies:
                    symbol_after_dot = state.symbol_after_dot(body);
                    # A -> a DOT
                    if symbol_after_dot is None:
                        # # S' -> S DOT
                        if prod.head == goal_symbol:
                            action = Action(action_type=Action.ACCEPT)
                            key = (state, eof_symbol)
                            if key in self.action_table:
                                    raise Exception('Oops, action conflit!')

                            self.action_table[key] = action

                        else:
                            reduce_prod = Production()
                            reduce_prod.head = prod.head
                            reduce_prod.bodies = [body]
                            action = Action(action_type=Action.REDUCE, reduce_prod=reduce_prod)
    
                            follow_A = self.grammar.follow[prod.head]
                            for s in follow_A:
                                key = (state, s)
                                if key in self.action_table:
                                    raise Exception('Oops, action conflit! ({}, {}) -> {}, {}'.format(state, s, self.action_table[key], action))
    
                                self.action_table[key] = action
                    
                    elif symbol_after_dot.is_terminal:
                        key = (state, symbol_after_dot)
                        shift_state = self.goto_table[key]
                        assert shift_state is not None

                        action = Action(action_type=Action.SHIFT, shift_state=shift_state)

                        if key in self.action_table:
                                raise Exception('Oops, action conflit! ({}, {}) -> {}, {}'.format(state, symbol_after_dot, self.action_table[key], action))

                        self.action_table[key] = action

                    else:
                        key = (state, symbol_after_dot)
                        # if key in self.action_table:
                            # print('key:', key)
                            # print('action_table:', self.action_table)
                            # raise Exception('Oops, action conflit!')
                        if key not in self.action_table:
                            self.action_table[key] = Action.ERROR_ACTION


    def goto(self, state, symbol):
        assert isinstance(state, LR0Set) and isinstance(symbol, Symbol)

        key = (state, symbol)
        if key in self.goto_table:
            return self.goto_table[key]
        # print('goto(,{})'.format(symbol))
        target_items = []
        for item in state.items:
            item_copy = copy.deepcopy(item)

            bodies_to_remove = []
            for body in item_copy.production.bodies:
                symbol_after_dot = state.symbol_after_dot(body)
                # print('body: {} -> {}'.format(item.head, body))
                # print('symbol_after_dot:', symbol_after_dot)
                if symbol_after_dot != symbol:
                    bodies_to_remove.append(body)
            
            # print('remove list:', bodies_to_remove)
            for b in bodies_to_remove:
                # print('remove body:', b)
                item_copy.production.bodies.remove(b)

            if len(item_copy.production.bodies) > 0:
                # print('symbol: {}, item_copy:{}'.format(symbol, item_copy))
                self._dot_move_forward(item_copy.production)
                target_items.append(item_copy)

        # print('!!!!!!!!!!!!!!!!!!!!!!!target_items:', target_items)
        if len(target_items) == 0:
            self.goto_table[key] = None
        else:
            target_state = LR0Set(target_items, self.grammar)
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

    def action_table_str(self):
        ret_str = ''

        for key in self.action_table:
            action = self.action_table[key]
            if action is not None:
                if action == Action.ERROR_ACTION:
                    continue

                state, symbol = key
                ret_str += '({}, {}) -> {}\n'.format(state.id, symbol, action)
        
        return ret_str

class LR0Set(object):
    def __init__(self, items=None, grammar=None):
        self.items = items
        self.grammar = grammar
        self.id = -1

        self.closure()

    def closure(self):
        work_queue = copy.deepcopy(self.items)
        while len(work_queue) > 0:
            item = work_queue.pop()

            for body in item.production.bodies:
                symbol_after_dot = self.symbol_after_dot(body)
                if symbol_after_dot and not symbol_after_dot.is_terminal:
                    prods = self._prod_start_with(symbol_after_dot)
                    self._add_dot_to_body_front(prods)
                    
                    rest_body_index = body.index(symbol_after_dot) + 1
                    rest_body = body[rest_body_index:]

                    for new_prod in prods:
                        # print('body:', body)
                        items = self.new_items(new_prod, item, rest_body)
                        for item in items:
                            if item in self.items:
                                continue
                            # Add these items to set
                            self.items.append(item)
                            # Add these items to work queue
                            work_queue.append(item)

        # print('closure:\n{}'.format(self.items))
    
    # To be overrided:
    def new_items(self, production, item, rest_body):
        return [LR0Item(production)]

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
        return str(self.items)

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        return str(self) == str(other)

    def __hash__(self):
        return hash(tuple(self.items))

class LR0Item(object):
    def __init__(self, production=None):
        self.production = production

    def __str__(self):
        return str(self.production)

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        return self.production == other.production if other is not None else False

    def __hash__(self):
        return hash(self.production)

    def __deepcopy__(self, mem):
        return LR0Item(copy.deepcopy(self.production))

class Action(object):
    SHIFT = 0
    REDUCE = 1
    ACCEPT = 2
    ERROR = 3

    def __init__(self, action_type=None, shift_state=None, reduce_prod=None):
        self.action_type = action_type
        self.shift_state = shift_state
        self.reduce_prod = reduce_prod

    def __str__(self):
        ret = ''
        if self.action_type == self.SHIFT:
            ret += 'Shift {}'.format(self.shift_state.id)
        elif self.action_type == self.REDUCE:
            ret += 'Reduce {}'.format(self.reduce_prod)
        elif self.action_type == self.ACCEPT:
            ret += 'Accept'
        elif self.action_type == self.ERROR:
            ret += 'Error'
        else:
            raise Exception('Unknonw action type: {}'.format(self.action_type))

        return ret

    def __repr__(self):
        return str(self)

Action.ERROR_ACTION = Action(Action.ERROR)

class LR1Parser(LR0Parser):
    def initial_state(self):
        # Set0: closure([S' -> DOT S, EOF])
        prod = copy.deepcopy(self.grammar.productions[0])
        prod.bodies[0].insert(0, Symbol.DOT_SYMBOL)
        
        return LR1Set([LR1Item(prod, Symbol.EOF_SYMBOL)], self.grammar)

class LR1Set(LR0Set):
    def first(self, symbols):
        ret = set()
        for i, symbol in enumerate(symbols):
            first_set = self.grammar.first[symbol]
            ret.update(first_set)

            if Symbol.EPSILON_SYMBOL not in first_set:
                break

        if i == len(symbols):
            ret.add(Symbol.EPSILON_SYMBOL)

        return ret

    def new_items(self, production, item, rest_body):
        # print('production:', production)
        # print('rest_body:', rest_body)

        ret_items = set()
        for body in production.bodies:
            remaining_symbols = rest_body + [item.symbol]
    
            first_set = self.first(remaining_symbols)
    
            for symbol in first_set:
                ret_items.add(LR1Item(production, symbol))

        return ret_items

class LR1Item(LR0Item):
    def __init__(self, production=None, symbol=None):
        super(LR1Item, self).__init__(production)

        self.symbol = symbol

    def __str__(self):
        return '(' + str(self.production) + ', ' + str(self.symbol) + ')'

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        if other is None:
            return False

        return super(LR1Item, self).__eq__(other) and self.symbol == other.symbol

    def __hash__(self):
        return hash(self.production) + hash(self.symbol)

    def __deepcopy__(self, mem):
        return LR1Item(copy.deepcopy(self.production), copy.deepcopy(self.symbol))
