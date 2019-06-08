import unittest

from xcc.parser.context_free_grammar import *
from xcc.parser.grammar_factory import GrammarFactory
from xcc.parser.token import Token
from xcc.parser.ll_parser import LLParser

class LLParserTest(unittest.TestCase):
    def setUp(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')
        self.grammar.eliminate_left_recursion()
        self.grammar.compute_first_plus_set()

        self._prepare_input_sequence = self._prepare_input_sequence_for_partial_expression

    # Deprecated.
    def _prepare_input_sequence(self, input_stream):
        if not isinstance(input_stream, str):
            raise TypeError('"input_stream" is expected to be a str, but got {} instead.'.format(input_stream))
    
        token_seq = []
    
        for t in input_stream:
            if t != ' ':
                token_type = Symbol(t, is_terminal=True)
                token = Token(t, token_type)

                token_seq.append(token)

        # Add "EOF" to the end of the sequence.
        token_seq.append(Token("EOF", Symbol.EOF_SYMBOL))

        return token_seq

    def _prepare_input_sequence_for_partial_expression(self, input_stream):
        if not isinstance(input_stream, str):
            raise TypeError('"input_stream" is expected to be a str, but got {} instead.'.format(input_stream))
    
        token_seq = []
    
        for t in input_stream:
            if t == ' ':
                continue

            if t not in ('+', '*', '/', '-', '(', ')'):
                token_type = Symbol('id', is_terminal=True)
            else:
                token_type = Symbol(t, is_terminal=True)
            
            token = Token(t, token_type)
            token_seq.append(token)

        # Add "EOF" to the end of the sequence.
        token_seq.append(Token("EOF", Symbol.EOF_SYMBOL))

        return token_seq

    def test_ll_parsing(self):
        token_seq = self._prepare_input_sequence('1')
        parser = LLParser(self.grammar)
        ast = parser.parse(token_seq)

        actual = str(ast)
        
        expected = """
digraph
{
{0 [label="Goal"]} -> {1 [label="Expr"]};
{1 [label="Expr"]} -> {2 [label="Term"]};
{1 [label="Expr"]} -> {3 [label="Expr'"]};
{2 [label="Term"]} -> {4 [label="Factor"]};
{2 [label="Term"]} -> {5 [label="Term'"]};
{3 [label="Expr'"]} -> {6 [label="EPSILON"]};
{4 [label="Factor"]} -> {7 [label="id"]};
{5 [label="Term'"]} -> {8 [label="EPSILON"]};
{7 [label="id"]} -> {9 [label="1"]};
}
"""
        self.assertEqual(expected, actual)

    def test_ll_parsing_2(self):
        token_seq = self._prepare_input_sequence('1 + 2 * 3')
        parser = LLParser(self.grammar)
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
{4 [label="Factor"]} -> {9 [label="id"]};
{5 [label="Term'"]} -> {10 [label="EPSILON"]};
{6 [label="+"]} -> {11 [label="+"]};
{7 [label="Term"]} -> {12 [label="Factor"]};
{7 [label="Term"]} -> {13 [label="Term'"]};
{8 [label="Expr'"]} -> {14 [label="EPSILON"]};
{9 [label="id"]} -> {15 [label="1"]};
{12 [label="Factor"]} -> {16 [label="id"]};
{13 [label="Term'"]} -> {17 [label="*"]};
{13 [label="Term'"]} -> {18 [label="Factor"]};
{13 [label="Term'"]} -> {19 [label="Term'"]};
{16 [label="id"]} -> {20 [label="2"]};
{17 [label="*"]} -> {21 [label="*"]};
{18 [label="Factor"]} -> {22 [label="id"]};
{19 [label="Term'"]} -> {23 [label="EPSILON"]};
{22 [label="id"]} -> {24 [label="3"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_ll_parsing_3(self):
        token_seq = self._prepare_input_sequence('(1 + 1)')
        parser = LLParser(self.grammar)
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
{7 [label="("]} -> {11 [label="("]};
{8 [label="Expr"]} -> {12 [label="Term"]};
{8 [label="Expr"]} -> {13 [label="Expr'"]};
{9 [label=")"]} -> {14 [label=")"]};
{12 [label="Term"]} -> {15 [label="Factor"]};
{12 [label="Term"]} -> {16 [label="Term'"]};
{13 [label="Expr'"]} -> {17 [label="+"]};
{13 [label="Expr'"]} -> {18 [label="Term"]};
{13 [label="Expr'"]} -> {19 [label="Expr'"]};
{15 [label="Factor"]} -> {20 [label="id"]};
{16 [label="Term'"]} -> {21 [label="EPSILON"]};
{17 [label="+"]} -> {22 [label="+"]};
{18 [label="Term"]} -> {23 [label="Factor"]};
{18 [label="Term"]} -> {24 [label="Term'"]};
{19 [label="Expr'"]} -> {25 [label="EPSILON"]};
{20 [label="id"]} -> {26 [label="1"]};
{23 [label="Factor"]} -> {27 [label="id"]};
{24 [label="Term'"]} -> {28 [label="EPSILON"]};
{27 [label="id"]} -> {29 [label="1"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_ll_parsing_4(self):
        token_seq = self._prepare_input_sequence('(1)')
        parser = LLParser(self.grammar)
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
{7 [label="("]} -> {11 [label="("]};
{8 [label="Expr"]} -> {12 [label="Term"]};
{8 [label="Expr"]} -> {13 [label="Expr'"]};
{9 [label=")"]} -> {14 [label=")"]};
{12 [label="Term"]} -> {15 [label="Factor"]};
{12 [label="Term"]} -> {16 [label="Term'"]};
{13 [label="Expr'"]} -> {17 [label="EPSILON"]};
{15 [label="Factor"]} -> {18 [label="id"]};
{16 [label="Term'"]} -> {19 [label="EPSILON"]};
{18 [label="id"]} -> {20 [label="1"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_ll_parsing_5(self):
        token_seq = self._prepare_input_sequence('(1+2)*3')
        parser = LLParser(self.grammar)
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
{7 [label="("]} -> {13 [label="("]};
{8 [label="Expr"]} -> {14 [label="Term"]};
{8 [label="Expr"]} -> {15 [label="Expr'"]};
{9 [label=")"]} -> {16 [label=")"]};
{10 [label="*"]} -> {17 [label="*"]};
{11 [label="Factor"]} -> {18 [label="id"]};
{12 [label="Term'"]} -> {19 [label="EPSILON"]};
{14 [label="Term"]} -> {20 [label="Factor"]};
{14 [label="Term"]} -> {21 [label="Term'"]};
{15 [label="Expr'"]} -> {22 [label="+"]};
{15 [label="Expr'"]} -> {23 [label="Term"]};
{15 [label="Expr'"]} -> {24 [label="Expr'"]};
{18 [label="id"]} -> {25 [label="3"]};
{20 [label="Factor"]} -> {26 [label="id"]};
{21 [label="Term'"]} -> {27 [label="EPSILON"]};
{22 [label="+"]} -> {28 [label="+"]};
{23 [label="Term"]} -> {29 [label="Factor"]};
{23 [label="Term"]} -> {30 [label="Term'"]};
{24 [label="Expr'"]} -> {31 [label="EPSILON"]};
{26 [label="id"]} -> {32 [label="1"]};
{29 [label="Factor"]} -> {33 [label="id"]};
{30 [label="Term'"]} -> {34 [label="EPSILON"]};
{33 [label="id"]} -> {35 [label="2"]};
}
"""
        actual = str(ast)
        
        self.assertEqual(expected, actual)

    def test_ll_parsing_6(self):
        token_seq = self._prepare_input_sequence('1 * (2 + 3)')
        parser = LLParser(self.grammar)
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
{4 [label="Factor"]} -> {7 [label="id"]};
{5 [label="Term'"]} -> {8 [label="*"]};
{5 [label="Term'"]} -> {9 [label="Factor"]};
{5 [label="Term'"]} -> {10 [label="Term'"]};
{7 [label="id"]} -> {11 [label="1"]};
{8 [label="*"]} -> {12 [label="*"]};
{9 [label="Factor"]} -> {13 [label="("]};
{9 [label="Factor"]} -> {14 [label="Expr"]};
{9 [label="Factor"]} -> {15 [label=")"]};
{10 [label="Term'"]} -> {16 [label="EPSILON"]};
{13 [label="("]} -> {17 [label="("]};
{14 [label="Expr"]} -> {18 [label="Term"]};
{14 [label="Expr"]} -> {19 [label="Expr'"]};
{15 [label=")"]} -> {20 [label=")"]};
{18 [label="Term"]} -> {21 [label="Factor"]};
{18 [label="Term"]} -> {22 [label="Term'"]};
{19 [label="Expr'"]} -> {23 [label="+"]};
{19 [label="Expr'"]} -> {24 [label="Term"]};
{19 [label="Expr'"]} -> {25 [label="Expr'"]};
{21 [label="Factor"]} -> {26 [label="id"]};
{22 [label="Term'"]} -> {27 [label="EPSILON"]};
{23 [label="+"]} -> {28 [label="+"]};
{24 [label="Term"]} -> {29 [label="Factor"]};
{24 [label="Term"]} -> {30 [label="Term'"]};
{25 [label="Expr'"]} -> {31 [label="EPSILON"]};
{26 [label="id"]} -> {32 [label="2"]};
{29 [label="Factor"]} -> {33 [label="id"]};
{30 [label="Term'"]} -> {34 [label="EPSILON"]};
{33 [label="id"]} -> {35 [label="3"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_ll_parsing_7(self):
        token_seq = self._prepare_input_sequence('(1 + 2) * (3 + 4)')
        parser = LLParser(self.grammar)
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
{7 [label="("]} -> {13 [label="("]};
{8 [label="Expr"]} -> {14 [label="Term"]};
{8 [label="Expr"]} -> {15 [label="Expr'"]};
{9 [label=")"]} -> {16 [label=")"]};
{10 [label="*"]} -> {17 [label="*"]};
{11 [label="Factor"]} -> {18 [label="("]};
{11 [label="Factor"]} -> {19 [label="Expr"]};
{11 [label="Factor"]} -> {20 [label=")"]};
{12 [label="Term'"]} -> {21 [label="EPSILON"]};
{14 [label="Term"]} -> {22 [label="Factor"]};
{14 [label="Term"]} -> {23 [label="Term'"]};
{15 [label="Expr'"]} -> {24 [label="+"]};
{15 [label="Expr'"]} -> {25 [label="Term"]};
{15 [label="Expr'"]} -> {26 [label="Expr'"]};
{18 [label="("]} -> {27 [label="("]};
{19 [label="Expr"]} -> {28 [label="Term"]};
{19 [label="Expr"]} -> {29 [label="Expr'"]};
{20 [label=")"]} -> {30 [label=")"]};
{22 [label="Factor"]} -> {31 [label="id"]};
{23 [label="Term'"]} -> {32 [label="EPSILON"]};
{24 [label="+"]} -> {33 [label="+"]};
{25 [label="Term"]} -> {34 [label="Factor"]};
{25 [label="Term"]} -> {35 [label="Term'"]};
{26 [label="Expr'"]} -> {36 [label="EPSILON"]};
{28 [label="Term"]} -> {37 [label="Factor"]};
{28 [label="Term"]} -> {38 [label="Term'"]};
{29 [label="Expr'"]} -> {39 [label="+"]};
{29 [label="Expr'"]} -> {40 [label="Term"]};
{29 [label="Expr'"]} -> {41 [label="Expr'"]};
{31 [label="id"]} -> {42 [label="1"]};
{34 [label="Factor"]} -> {43 [label="id"]};
{35 [label="Term'"]} -> {44 [label="EPSILON"]};
{37 [label="Factor"]} -> {45 [label="id"]};
{38 [label="Term'"]} -> {46 [label="EPSILON"]};
{39 [label="+"]} -> {47 [label="+"]};
{40 [label="Term"]} -> {48 [label="Factor"]};
{40 [label="Term"]} -> {49 [label="Term'"]};
{41 [label="Expr'"]} -> {50 [label="EPSILON"]};
{43 [label="id"]} -> {51 [label="2"]};
{45 [label="id"]} -> {52 [label="3"]};
{48 [label="Factor"]} -> {53 [label="id"]};
{49 [label="Term'"]} -> {54 [label="EPSILON"]};
{53 [label="id"]} -> {55 [label="4"]};
}
"""
        actual = str(ast)

        self.assertEqual(expected, actual)

    def test_ll_parsing_8(self):
        token_seq = self._prepare_input_sequence('(1')
        parser = LLParser(self.grammar)

        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_9(self):
        token_seq = self._prepare_input_sequence('(1 +')
        parser = LLParser(self.grammar)
        
        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_10(self):
        token_seq = self._prepare_input_sequence('1*')
        parser = LLParser(self.grammar)

        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_11(self):
        token_seq = self._prepare_input_sequence('1*(')
        parser = LLParser(self.grammar)
        
        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_12(self):
        token_seq = self._prepare_input_sequence('(1')
        parser = LLParser(self.grammar)
        
        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_13(self):
        token_seq = self._prepare_input_sequence('(1 + 2) * (3')
        parser = LLParser(self.grammar)
        
        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)

    def test_ll_parsing_14(self):
        token_seq = self._prepare_input_sequence('(1 + 2) * (3 + 4')
        parser = LLParser(self.grammar)
        
        with self.assertRaises(Exception):
            ast = parser.parse(token_seq)