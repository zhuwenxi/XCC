import re
from functools import partial
from ..parser.token import Token
from ..parser.context_free_grammar import Symbol

def get_token(text, type=''):
    token_type = Symbol(type, is_terminal=True)
    return Token(text, token_type)

ID = partial(get_token, type='id')
KEYWORD = partial(get_token, type='keyword')
DECIMAL = partial(get_token, type='decimal')
OCTAL = partial(get_token, type='octal')
HEXADECIMAL = partial(get_token, type='hexadecimal')
CHAR = partial(get_token, type='char')
FLOAT = partial(get_token, type='float')
STRING = partial(get_token, type='string')
OPERATOR = partial(get_token, type='operator')

C_LEX = {
    # Keyword:
    'auto' : KEYWORD,
    'double' : KEYWORD,
    'int' : KEYWORD,
    'struct' : KEYWORD,
    'break' : KEYWORD,
    'else' : KEYWORD,
    'long' : KEYWORD,
    'switch' : KEYWORD,
    'case' : KEYWORD,
    'enum' : KEYWORD,
    'register' : KEYWORD,
    'typedef' : KEYWORD,
    'char' : KEYWORD,
    'extern' : KEYWORD,
    'return' : KEYWORD,
    'union' : KEYWORD,
    'const' : KEYWORD,
    'float' : KEYWORD,
    'short' : KEYWORD,
    'unsigned' : KEYWORD,
    'continue' : KEYWORD,
    'for' : KEYWORD,
    'signed' : KEYWORD,
    'void' : KEYWORD,
    'default' : KEYWORD,
    'goto' : KEYWORD,
    'sizeof' : KEYWORD,
    'volatile' : KEYWORD,
    'do' : KEYWORD,
    'if' : KEYWORD,
    'static' : KEYWORD,
    'while' : KEYWORD,
    'fortran' : KEYWORD,
    'asm' : KEYWORD,

    # Identifer:
    '[_a-zA-Z][_a-zA-Z0-9]*' : ID,

    # Decimal:
    '[0-9]([uUlL]*)' : DECIMAL,
    '[1-9][0-9]*([uUlL]*)' : DECIMAL,

    # Octal:
    '0[0-7]+([uUlL]*)' : OCTAL,

    # Hexadecimal:
    '(0x|0X)[0-9a-fA-F]+([uUlL]*)' : HEXADECIMAL,

    # Char:
    "'[a-zA-Z]'" : CHAR,
    "'\n'" : CHAR,
    "'\\t'" : CHAR,
    "'\\v'" : CHAR,
    "'\\b'" : CHAR,
    "'\\r'" : CHAR,
    "'\\a'" : CHAR,
    "'\\\\'" : CHAR,
    "'\\?'" : CHAR,
    "'\''" : CHAR,
    "'\"'" : CHAR,
    "'\f'" : CHAR,
    "'\\[0-7][0-7][0-7]'" : CHAR,
    "'\\x[0-9a-fA-F][0-9a-fA-F]'" : CHAR,

    # Float:
    '[+-]?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+(\.[0-9]+)?)?([uUlL]*)' : FLOAT,

    # String constant:
    '"[a-zA-Z]*"' : STRING,

    # Operator:
    '\+\+' : OPERATOR,
    '\-\-' : OPERATOR,
    '\+' : OPERATOR,
    '\-' : OPERATOR,
    '\*' : OPERATOR,
    '/' : OPERATOR,
    '%' : OPERATOR,
    '.' : OPERATOR,
    '->' : OPERATOR,
    '<<' : OPERATOR,
    '>>' : OPERATOR,
    '&' : OPERATOR,
    '!' : OPERATOR,
    '^' : OPERATOR,
    '~' : OPERATOR,
    '&&' : OPERATOR,
    '|' : OPERATOR,
    '||' : OPERATOR,
    '>' : OPERATOR,
    '<' : OPERATOR,
    '<=' : OPERATOR,
    '>=' : OPERATOR,
    '==' : OPERATOR,
    '!=' : OPERATOR,
    '\?' : OPERATOR,
    ':' : OPERATOR,
    ',' : OPERATOR,
    ';' : OPERATOR,
    '=' : OPERATOR,
    '\*=' : OPERATOR,
    '\/=' : OPERATOR,
    '%=' : OPERATOR,
    '+=' : OPERATOR,
    '-=' : OPERATOR,
    '<<=' : OPERATOR,
    '>>=' : OPERATOR,
    '&=' : OPERATOR,
    '^=' : OPERATOR,
    '|=' : OPERATOR,

}

class IllFormInput(Exception):
    pass

def tokenize(text):
    tokens = []
    
    cursor = 0

    while cursor < len(text):
        for pattern in C_LEX:
            cand = re.match(pattern, text[cursor:])

            if cand is not None:
                token_str = cand.group(0)
                cursor += len(token_str)

                token = C_LEX[pattern](token_str)
                tokens.append(token)

                break

        if cand is None:
            raise IllFormInput('Ill input:', text[cursor:])

    return tokens

def scan(filename):
    with open(filename) as f:
        content = f.read()
        return tokenize(content)