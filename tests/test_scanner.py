import unittest
import re

from xcc.scanner import scan

class ScannerTest(unittest.TestCase):
    def test_identifiers(self):
        match = re.match('[_a-zA-Z][_a-zA-Z0-9]*', 'idx3')

        self.assertEqual(match.group(0), 'idx3')

    def test_decimal(self):
        match = re.match('[1-9][0-9]*([uUlL]*)', '100L')

        self.assertEqual(match.group(0), '100L')

    def test_char(self):
        match = re.match("'\\n'", "'\n'")
        self.assertEqual(match.group(0), '\\n')


    def test_scanner(self):
        tokens = scan('./c/main.c')
        print(tokens)