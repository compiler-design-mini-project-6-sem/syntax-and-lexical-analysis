#include <iostream>
#include <vector>
#include "utils/token.h"
using namespace std;

// Forward declarations
vector<Token> tokenize(string filename);
void saveTokens(vector<Token>& tokens);
void parse(vector<Token>& tokens);

int main() {

    string file = "input/input.txt";

    // Step 1: Lexical Analysis
    vector<Token> tokens = tokenize(file);
    saveTokens(tokens);

    cout << "Lexical Analysis Done\n";

    // Step 2: Syntax Analysis
    parse(tokens);

    cout << "Syntax Analysis Done\n";

    return 0;
}