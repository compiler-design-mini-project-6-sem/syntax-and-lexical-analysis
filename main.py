from lexer import tokenize
from parser_module import print_parser_data, run_ll1_simulation
from tabulate import tabulate
import sys

source_code = """
BEGIN
PRINT "HELLO" 
INTEGER A, B, C 
REAL D, E 
STRING X, Y
A := 2
B := 4
C := 6
D := -3.56E-8
E := 4.567
X := "text1"
Y := "hello there" 
FOR I := 1 TO 5
PRINT "Strings are [X] and [Y]" 
END
"""

# Run lexer
tokens, keywords, identifiers, literals = tokenize(source_code)


# ---------- FILE 1: TOKEN + SYMBOL TABLES ----------
with open("output_tokens.txt", "w") as f:
    sys.stdout = f

    print("TOKEN TABLE:")
    print(tabulate(tokens, headers=["Type", "Lexeme", "Token ID"], tablefmt="fancy_grid"))

    print("\nSYMBOL TABLE - Keywords:")
    print(tabulate([[k] for k in sorted(keywords)], headers=["Keyword"], tablefmt="fancy_grid"))

    print("\nSYMBOL TABLE - Identifiers:")
    print(tabulate([[i] for i in sorted(identifiers)], headers=["Identifier"], tablefmt="fancy_grid"))

    print("\nSYMBOL TABLE - Literals:")
    print(tabulate([[l] for l in sorted(literals)], headers=["Literal"], tablefmt="fancy_grid"))


# ---------- FILE 2: GRAMMAR + FIRST + FOLLOW ----------
with open("output_parser.txt", "w") as f:
    sys.stdout = f
    print_parser_data()


# ---------- FILE 3: LL(1) PARSING STEPS ----------
with open("output_parsing_steps.txt", "w") as f:
    sys.stdout = f
    run_ll1_simulation()


# Reset stdout back to normal
sys.stdout = sys.__stdout__

print("All outputs saved successfully.")