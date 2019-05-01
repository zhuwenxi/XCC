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

    def _prepare_input_sequence(self, input_stream):
        if not isinstance(input_stream, str):
            raise TypeError('"input_stream" is expected to be a str, but got {} instead.'.format(input_stream))
    
        token_seq = []
    
        for t in input_stream:
            if t == ' ':
                continue

            if t not in ('+', '*'):
                token_type = Symbol('id', is_terminal=True)
            else:
                token_type = Symbol(t, is_terminal=True)
            
            token = Token(t, token_type)
            token_seq.append(token)

        # Add "EOF" to the end of the sequence.
        token_seq.append(Token("EOF", Symbol.EOF_SYMBOL))

        return token_seq

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

    def test_lr_0_parsing_tables_construct(self):
        grammar = GrammarFactory.get_grammar('partial-expression')
        grammar.compute_first_plus_set()
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

        actual_action_table = lr_0_parser.action_table_str()

        expected_action_table = """\
(0, () -> Shift 4
(0, id) -> Shift 5
(1, EOF) -> Accept
(1, +) -> Shift 6
(2, )) -> Reduce E -> T DOT
(2, +) -> Reduce E -> T DOT
(2, EOF) -> Reduce E -> T DOT
(2, *) -> Shift 7
(3, )) -> Reduce T -> F DOT
(3, *) -> Reduce T -> F DOT
(3, +) -> Reduce T -> F DOT
(3, EOF) -> Reduce T -> F DOT
(4, () -> Shift 4
(4, id) -> Shift 5
(5, )) -> Reduce F -> id DOT
(5, *) -> Reduce F -> id DOT
(5, +) -> Reduce F -> id DOT
(5, EOF) -> Reduce F -> id DOT
(6, () -> Shift 4
(6, id) -> Shift 5
(7, () -> Shift 4
(7, id) -> Shift 5
(8, )) -> Shift 11
(8, +) -> Shift 6
(9, )) -> Reduce E -> E + T DOT
(9, +) -> Reduce E -> E + T DOT
(9, EOF) -> Reduce E -> E + T DOT
(9, *) -> Shift 7
(10, )) -> Reduce T -> T * F DOT
(10, *) -> Reduce T -> T * F DOT
(10, +) -> Reduce T -> T * F DOT
(10, EOF) -> Reduce T -> T * F DOT
(11, )) -> Reduce F -> ( E ) DOT
(11, *) -> Reduce F -> ( E ) DOT
(11, +) -> Reduce F -> ( E ) DOT
(11, EOF) -> Reduce F -> ( E ) DOT
"""
        self.assertEqual(expected_action_table, actual_action_table)

    def test_lr_0_parsing(self):
        grammar = GrammarFactory.get_grammar('partial-expression')
        grammar.compute_first_plus_set()
        lr_0_parser = LR0Parser(grammar)

        token_seq = self._prepare_input_sequence('1 + 2')

        lr_0_parser.parse(token_seq)


