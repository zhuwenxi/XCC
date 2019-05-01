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

    def test_lr_0_parsing_goto_table_construct(self):
        grammar = GrammarFactory.get_grammar('partial-expression')
        lr_0_parser = LR0Parser(grammar)
        actual = lr_0_parser.goto_table_str()

        expected = """
digraph
{
0 -> 1 [label="E"];
0 -> 2 [label="T"];
0 -> 3 [label="F"];
0 -> 4 [label="("];
0 -> 5 [label="id"];
1 -> 6 [label="+"];
2 -> 7 [label="*"];
4 -> 8 [label="E"];
4 -> 2 [label="T"];
4 -> 3 [label="F"];
4 -> 4 [label="("];
4 -> 5 [label="id"];
6 -> 9 [label="T"];
6 -> 3 [label="F"];
6 -> 4 [label="("];
6 -> 5 [label="id"];
7 -> 10 [label="F"];
7 -> 4 [label="("];
7 -> 5 [label="id"];
8 -> 11 [label=")"];
8 -> 6 [label="+"];
9 -> 7 [label="*"];
}"""
        self.assertEqual(expected, actual)
        
