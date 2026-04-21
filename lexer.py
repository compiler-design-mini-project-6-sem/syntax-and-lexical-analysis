import re

# Define language
keywords = {'BEGIN', 'PRINT', 'INTEGER', 'REAL', 'STRING', 'FOR', 'TO', 'END'}
operators = {'+', '-', '*', '/', ':=', ';', ','}
relops = {'<', '>', '<=', '>=', '==', '!='}
whitespace = r'[ \t]+'

# Token specification
token_specification = [
('KEYWORD', r'\b(?:BEGIN|PRINT|INTEGER|REAL|STRING|FOR|TO|END)\b'),
('NUMBER', r'-?\d+\.?\d*(?:[Ee][+-]?\d+)?'),
('STRING', r'"[^"]*"'),
('IDENTIFIER', r'\b[a-zA-Z_]\w*\b'),
('OPERATOR', r':=|;|,'),
('RELOP', r'<=|>=|==|!=|<|>'),
('NEWLINE', r'\n'),
('SKIP', whitespace),
('MISMATCH', r'.'),
]

tok_regex = '|'.join(f'(?P<{name}>{pattern})' for name, pattern in token_specification)


def tokenize(source_code):
    tokens = []
    token_values = {}
    current_token_id = 1

    keywords_in_code = set()
    identifiers_in_code = set()
    literals_in_code = set()

    for mo in re.finditer(tok_regex, source_code):
        kind = mo.lastgroup
        value = mo.group()

        if kind in ['SKIP', 'NEWLINE']:
            continue

        if kind == 'MISMATCH':
            raise RuntimeError(f'Unexpected character {value!r}')

        if kind == 'IDENTIFIER' and value in keywords:
            kind = 'KEYWORD'

        if value not in token_values:
            token_values[value] = current_token_id
            current_token_id += 1

        tokens.append((kind, value, token_values[value]))

        if kind == 'KEYWORD':
            keywords_in_code.add(value)
        elif kind == 'IDENTIFIER':
            identifiers_in_code.add(value)
        elif kind in ['NUMBER', 'STRING']:
            literals_in_code.add(value)

    return tokens, keywords_in_code, identifiers_in_code, literals_in_code