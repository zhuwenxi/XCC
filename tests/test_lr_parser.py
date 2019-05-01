import unittest

from xcc.scanner.context_free_grammar import *
from xcc.scanner.grammar_factory import GrammarFactory
from xcc.scanner.token import Token
from xcc.scanner.lr_parser import *




class LRParserTest(unittest.TestCase):
    def setUp(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')
        self.grammar.compute_first_plus_set()

    def test_lr_0_set(self):
        # Goal -> DOT Expr
        prod = copy.deepcopy(self.grammar.productions[0])
        prod.bodies[0].insert(0, Symbol.DOT_SYMBOL)
        lr_0_set = LR0Set([prod], self.grammar)

        expected = "[Goal -> DOT Expr, "\
                   "Expr -> DOT Expr + Term | DOT Expr - Term | DOT Term, "\
                   "Term -> DOT Term * Factor | DOT Term / Factor | DOT Factor, "\
                   "Factor -> DOT ( Expr ) | DOT 0 | DOT 1 | DOT 2 | "\
                   "DOT 3 | DOT 4 | DOT 5 | DOT 6 | DOT 7 | DOT 8 | DOT 9]"

        actual = str(lr_0_set)

        self.assertEqual(expected, actual)

    def test_lr_0_parser_create(self):
        lr_0_parser = LR0Parser(self.grammar)
        # print(lr_0_parser.states)
