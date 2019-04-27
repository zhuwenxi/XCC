import unittest
from xcc.scanner.context_free_grammar import *
from xcc.scanner.grammar_factory import GrammarFactory


class GrammarTest(unittest.TestCase):
    def setUp(self):
        self.grammar = GrammarFactory.get_grammar('expression')

    def test_classic_expression_grammar(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')

        expected = 'Goal -> Expr\n'\
                   'Expr -> Expr + Term | Expr - Term | Term\n'\
                   'Term -> Term * Factor | Term / Factor | Factor\n'\
                   'Factor -> ( Expr ) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n'

        actual = str(self.grammar)

        self.assertEqual(actual, expected)

    def test_eliminate_left_recursion(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')

        expected = "Goal -> Expr\n"\
                   "Expr -> Term Expr'\n"\
                   "Expr' -> + Term Expr' | - Term Expr' | EPSILON\n"\
                   "Term -> Factor Term'\n"\
                   "Term' -> * Factor Term' | / Factor Term' | EPSILON\n"\
                   "Factor -> ( Expr ) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
        
        self.grammar.eliminate_left_recursion()
        actual = str(self.grammar)

        self.assertEqual(expected, actual)

    

    def test_compute_first_set(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')

        self.grammar.eliminate_left_recursion()
        self.grammar.compute_first_set()

        actual = str(self.grammar.first)

        expected = "{Goal: [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "Expr: [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "Term: [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "Expr': [+, -, EPSILON], "\
                   "+: [+], "\
                   "-: [-], "\
                   "EPSILON: [EPSILON], "\
                   "Factor: [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "Term': [*, /, EPSILON], "\
                   "*: [*], "\
                   "/: [/], "\
                   "(: [(], "\
                   "): [)], "\
                   "0: [0], "\
                   "1: [1], "\
                   "2: [2], "\
                   "3: [3], "\
                   "4: [4], "\
                   "5: [5], "\
                   "6: [6], "\
                   "7: [7], "\
                   "8: [8], "\
                   "9: [9]}"

        self.assertEqual(expected, actual)

    def test_compute_follow_set(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')
        self.grammar.eliminate_left_recursion()

        self.grammar.compute_follow_set()

        actual = str(self.grammar.follow)

        expected = "{Goal: [EOF], "\
                   "Expr: [), EOF], "\
                   "Term: [), +, -, EOF], "\
                   "Expr': [), EOF], "\
                   "Factor: [), *, +, -, /, EOF], "\
                   "Term': [), +, -, EOF]}"\
        
        self.assertEqual(expected, actual)

    def test_compute_first_plus_set(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')
        self.grammar.eliminate_left_recursion()

        self.grammar.compute_first_plus_set()

        actual = str(self.grammar.first_plus)

        expected = "{(Goal, (Expr,)): [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "(Expr, (Term, Expr')): [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "(Expr', (+, Term, Expr')): [+], "\
                   "(Expr', (-, Term, Expr')): [-], "\
                   "(Expr', (EPSILON,)): [), EOF, EPSILON], "\
                   "(Term, (Factor, Term')): [(, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "\
                   "(Term', (*, Factor, Term')): [*], "\
                   "(Term', (/, Factor, Term')): [/], "\
                   "(Term', (EPSILON,)): [), +, -, EOF, EPSILON], "\
                   "(Factor, ((, Expr, ))): [(], "\
                   "(Factor, (0,)): [0], "\
                   "(Factor, (1,)): [1], "\
                   "(Factor, (2,)): [2], "\
                   "(Factor, (3,)): [3], "\
                   "(Factor, (4,)): [4], "\
                   "(Factor, (5,)): [5], "\
                   "(Factor, (6,)): [6], "\
                   "(Factor, (7,)): [7], "\
                   "(Factor, (8,)): [8], "\
                   "(Factor, (9,)): [9]}"

        self.assertEqual(expected, actual)

    def test_grammar_check_backtrace_free_positive_case(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')
        self.grammar.eliminate_left_recursion()

        actual = self.grammar.check_backtrace_free()
        
        expected = True

    def test_grammar_check_backtrace_free_negative_case(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('ifelse')

        actual = self.grammar.check_backtrace_free()
        
        expected = False

        self.assertEqual(expected, actual)

if __name__ == '__main__':
    unittest.main()
