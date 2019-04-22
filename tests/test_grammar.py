import unittest
from xcc.scanner.context_free_grammar import *
from xcc.scanner.grammar_factory import GrammarFactory
from xcc.scanner.token import Token
from xcc.scanner.recursive_decent_parser import RecusiveDecentParser


class GrammarTest(unittest.TestCase):
    def setUp(self):
        self.grammar = GrammarFactory.get_grammar('expression')
    
    def test_classic_expression_grammar(self):
        expected = 'Goal -> Expr\n'\
                   'Expr -> Expr + Term | Expr - Term | Term\n'\
                   'Term -> Term * Factor | Term / Factor | Factor\n'\
                   'Factor -> ( Expr ) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n'

        actual = str(self.grammar)

        self.assertEqual(actual, expected)

    def test_eliminate_left_recursion(self):
        expected = "Goal -> Expr\n"\
                   "Expr -> Term Expr'\n"\
                   "Expr' -> + Term Expr' | - Term Expr' | EPSILON\n"\
                   "Term -> Factor Term'\n"\
                   "Term' -> * Factor Term' | / Factor Term' | EPSILON\n"\
                   "Factor -> ( Expr ) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
        
        self.grammar.eliminate_left_recursion()
        actual = str(self.grammar)

        self.assertEqual(expected, actual)

    def _prepare_input_sequence(self, input_stream):
        if not isinstance(input_stream, str):
            raise TypeError('"input_stream" is expected to be a str, but got {} instead.'.format(input_stream))
    
        token_seq = []
    
        for t in input_stream:
            if t != ' ':
                token_type = Symbol(t, is_terminal=True)
                token = Token(t, token_type)

                token_seq.append(token)

        return token_seq

    def test_recursive_descent_parsing(self):
        token_seq = self._prepare_input_sequence('1')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)


        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="1"]};
{5 [label="Term'"]} -> {8 [label="EPSILON"]};
}
"""

        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_1(self):
        token_seq = self._prepare_input_sequence('1 + 2')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="+"]};
{3 [label="Expr'"]} -> {7 [label="Term"]};
{3 [label="Expr'"]} -> {8 [label="Expr'"]};
{4 [label="Factor"]} -> {9 [label="1"]};
{5 [label="Term'"]} -> {10 [label="EPSILON"]};
{7 [label="Term"]} -> {11 [label="Factor"]};
{7 [label="Term"]} -> {12 [label="Term'"]};
{8 [label="Expr'"]} -> {13 [label="EPSILON"]};
{11 [label="Factor"]} -> {14 [label="2"]};
{12 [label="Term'"]} -> {15 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_2(self):
        token_seq = self._prepare_input_sequence('1 + 2 * 3')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="+"]};
{3 [label="Expr'"]} -> {7 [label="Term"]};
{3 [label="Expr'"]} -> {8 [label="Expr'"]};
{4 [label="Factor"]} -> {9 [label="1"]};
{5 [label="Term'"]} -> {10 [label="EPSILON"]};
{7 [label="Term"]} -> {11 [label="Factor"]};
{7 [label="Term"]} -> {12 [label="Term'"]};
{8 [label="Expr'"]} -> {13 [label="EPSILON"]};
{11 [label="Factor"]} -> {14 [label="2"]};
{12 [label="Term'"]} -> {15 [label="*"]};
{12 [label="Term'"]} -> {16 [label="Factor"]};
{12 [label="Term'"]} -> {17 [label="Term'"]};
{16 [label="Factor"]} -> {18 [label="3"]};
{17 [label="Term'"]} -> {19 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_3(self):
        token_seq = self._prepare_input_sequence('(1 + 1)')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="("]};
{4 [label="Factor"]} -> {8 [label="Expr"]};
{4 [label="Factor"]} -> {9 [label=")"]};
{5 [label="Term'"]} -> {10 [label="EPSILON"]};
{8 [label="Expr"]} -> {11 [label="Term"]};
{8 [label="Expr"]} -> {12 [label="Expr'"]};
{11 [label="Term"]} -> {13 [label="Factor"]};
{11 [label="Term"]} -> {14 [label="Term'"]};
{12 [label="Expr'"]} -> {15 [label="+"]};
{12 [label="Expr'"]} -> {16 [label="Term"]};
{12 [label="Expr'"]} -> {17 [label="Expr'"]};
{13 [label="Factor"]} -> {18 [label="1"]};
{14 [label="Term'"]} -> {19 [label="EPSILON"]};
{16 [label="Term"]} -> {20 [label="Factor"]};
{16 [label="Term"]} -> {21 [label="Term'"]};
{17 [label="Expr'"]} -> {22 [label="EPSILON"]};
{20 [label="Factor"]} -> {23 [label="1"]};
{21 [label="Term'"]} -> {24 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_4(self):
        token_seq = self._prepare_input_sequence('(1)')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="("]};
{4 [label="Factor"]} -> {8 [label="Expr"]};
{4 [label="Factor"]} -> {9 [label=")"]};
{5 [label="Term'"]} -> {10 [label="EPSILON"]};
{8 [label="Expr"]} -> {11 [label="Term"]};
{8 [label="Expr"]} -> {12 [label="Expr'"]};
{11 [label="Term"]} -> {13 [label="Factor"]};
{11 [label="Term"]} -> {14 [label="Term'"]};
{12 [label="Expr'"]} -> {15 [label="EPSILON"]};
{13 [label="Factor"]} -> {16 [label="1"]};
{14 [label="Term'"]} -> {17 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_5(self):
        token_seq = self._prepare_input_sequence('(1+2)*3')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="("]};
{4 [label="Factor"]} -> {8 [label="Expr"]};
{4 [label="Factor"]} -> {9 [label=")"]};
{5 [label="Term'"]} -> {10 [label="*"]};
{5 [label="Term'"]} -> {11 [label="Factor"]};
{5 [label="Term'"]} -> {12 [label="Term'"]};
{8 [label="Expr"]} -> {13 [label="Term"]};
{8 [label="Expr"]} -> {14 [label="Expr'"]};
{11 [label="Factor"]} -> {15 [label="3"]};
{12 [label="Term'"]} -> {16 [label="EPSILON"]};
{13 [label="Term"]} -> {17 [label="Factor"]};
{13 [label="Term"]} -> {18 [label="Term'"]};
{14 [label="Expr'"]} -> {19 [label="+"]};
{14 [label="Expr'"]} -> {20 [label="Term"]};
{14 [label="Expr'"]} -> {21 [label="Expr'"]};
{17 [label="Factor"]} -> {22 [label="1"]};
{18 [label="Term'"]} -> {23 [label="EPSILON"]};
{20 [label="Term"]} -> {24 [label="Factor"]};
{20 [label="Term"]} -> {25 [label="Term'"]};
{21 [label="Expr'"]} -> {26 [label="EPSILON"]};
{24 [label="Factor"]} -> {27 [label="2"]};
{25 [label="Term'"]} -> {28 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_6(self):
        token_seq = self._prepare_input_sequence('1 * (2 + 3)')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="1"]};
{5 [label="Term'"]} -> {8 [label="*"]};
{5 [label="Term'"]} -> {9 [label="Factor"]};
{5 [label="Term'"]} -> {10 [label="Term'"]};
{9 [label="Factor"]} -> {11 [label="("]};
{9 [label="Factor"]} -> {12 [label="Expr"]};
{9 [label="Factor"]} -> {13 [label=")"]};
{10 [label="Term'"]} -> {14 [label="EPSILON"]};
{12 [label="Expr"]} -> {15 [label="Term"]};
{12 [label="Expr"]} -> {16 [label="Expr'"]};
{15 [label="Term"]} -> {17 [label="Factor"]};
{15 [label="Term"]} -> {18 [label="Term'"]};
{16 [label="Expr'"]} -> {19 [label="+"]};
{16 [label="Expr'"]} -> {20 [label="Term"]};
{16 [label="Expr'"]} -> {21 [label="Expr'"]};
{17 [label="Factor"]} -> {22 [label="2"]};
{18 [label="Term'"]} -> {23 [label="EPSILON"]};
{20 [label="Term"]} -> {24 [label="Factor"]};
{20 [label="Term"]} -> {25 [label="Term'"]};
{21 [label="Expr'"]} -> {26 [label="EPSILON"]};
{24 [label="Factor"]} -> {27 [label="3"]};
{25 [label="Term'"]} -> {28 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_recursive_descent_parsing_7(self):
        token_seq = self._prepare_input_sequence('(1 + 2) * (3 + 4)')
        parser = RecusiveDecentParser(self.grammar)
        ast = parser.parse(token_seq)

        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="("]};
{4 [label="Factor"]} -> {8 [label="Expr"]};
{4 [label="Factor"]} -> {9 [label=")"]};
{5 [label="Term'"]} -> {10 [label="*"]};
{5 [label="Term'"]} -> {11 [label="Factor"]};
{5 [label="Term'"]} -> {12 [label="Term'"]};
{8 [label="Expr"]} -> {13 [label="Term"]};
{8 [label="Expr"]} -> {14 [label="Expr'"]};
{11 [label="Factor"]} -> {15 [label="("]};
{11 [label="Factor"]} -> {16 [label="Expr"]};
{11 [label="Factor"]} -> {17 [label=")"]};
{12 [label="Term'"]} -> {18 [label="EPSILON"]};
{13 [label="Term"]} -> {19 [label="Factor"]};
{13 [label="Term"]} -> {20 [label="Term'"]};
{14 [label="Expr'"]} -> {21 [label="+"]};
{14 [label="Expr'"]} -> {22 [label="Term"]};
{14 [label="Expr'"]} -> {23 [label="Expr'"]};
{16 [label="Expr"]} -> {24 [label="Term"]};
{16 [label="Expr"]} -> {25 [label="Expr'"]};
{19 [label="Factor"]} -> {26 [label="1"]};
{20 [label="Term'"]} -> {27 [label="EPSILON"]};
{22 [label="Term"]} -> {28 [label="Factor"]};
{22 [label="Term"]} -> {29 [label="Term'"]};
{23 [label="Expr'"]} -> {30 [label="EPSILON"]};
{24 [label="Term"]} -> {31 [label="Factor"]};
{24 [label="Term"]} -> {32 [label="Term'"]};
{25 [label="Expr'"]} -> {33 [label="+"]};
{25 [label="Expr'"]} -> {34 [label="Term"]};
{25 [label="Expr'"]} -> {35 [label="Expr'"]};
{28 [label="Factor"]} -> {36 [label="2"]};
{29 [label="Term'"]} -> {37 [label="EPSILON"]};
{31 [label="Factor"]} -> {38 [label="3"]};
{32 [label="Term'"]} -> {39 [label="EPSILON"]};
{34 [label="Term"]} -> {40 [label="Factor"]};
{34 [label="Term"]} -> {41 [label="Term'"]};
{35 [label="Expr'"]} -> {42 [label="EPSILON"]};
{40 [label="Factor"]} -> {43 [label="4"]};
{41 [label="Term'"]} -> {44 [label="EPSILON"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

if __name__ == '__main__':
    unittest.main()
