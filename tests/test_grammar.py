import unittest
from xcc.parser.context_free_grammar import *
from xcc.parser.grammar_factory import GrammarFactory


class GrammarTest(unittest.TestCase):
    def setUp(self):
        self.grammar = GrammarFactory.get_grammar('expression')
        self.maxDiff = None

    def test_classic_expression_grammar(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')

        expected = 'Goal -> Expr\n'\
                   'Expr -> Expr + Term | Expr - Term | Term\n'\
                   'Term -> Term * Factor | Term / Factor | Factor\n'\
                   'Factor -> ( Expr ) | id\n'

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
                   "Factor -> ( Expr ) | id\n"
        
        self.grammar.eliminate_left_recursion()
        actual = str(self.grammar)

        self.assertEqual(expected, actual)

    

    def test_compute_first_set(self):
        GrammarFactory.clear()
        self.grammar = GrammarFactory.get_grammar('expression')

        self.grammar.eliminate_left_recursion()
        self.grammar.compute_first_set()

        actual = str(self.grammar.first)

        expected = "{Goal: [(, id], "\
                   "Expr: [(, id], "\
                   "Term: [(, id], "\
                   "Expr': [+, -, EPSILON], "\
                   "+: [+], "\
                   "-: [-], "\
                   "EPSILON: [EPSILON], "\
                   "Factor: [(, id], "\
                   "Term': [*, /, EPSILON], "\
                   "*: [*], "\
                   "/: [/], "\
                   "(: [(], "\
                   "): [)], "\
                   "id: [id], "\
                   "EOF: [EOF]}"

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

        expected = "{(Goal, (Expr,)): [(, id], "\
                   "(Expr, (Term, Expr')): [(, id], "\
                   "(Expr', (+, Term, Expr')): [+], "\
                   "(Expr', (-, Term, Expr')): [-], "\
                   "(Expr', (EPSILON,)): [), EOF, EPSILON], "\
                   "(Term, (Factor, Term')): [(, id], "\
                   "(Term', (*, Factor, Term')): [*], "\
                   "(Term', (/, Factor, Term')): [/], "\
                   "(Term', (EPSILON,)): [), +, -, EOF, EPSILON], "\
                   "(Factor, ((, Expr, ))): [(], "\
                   "(Factor, (id,)): [id]}"

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
