#include <iostream>
#include <vector>
#include <fstream>
#include "utils/token.h"

using namespace std;

vector<Token> tokenize(string filename);
void parse(vector<Token>& tokens, ostream& out);

int main() {
    vector<Token> tokens = tokenize("input/input.txt");

    // Save tokens to lexical.txt
    ofstream lexFile("output/lexical.txt");
    lexFile << "TOKENS:\n";
    for (auto &t : tokens) {
        lexFile << t.value << " -> " << t.type << endl;
    }
    lexFile.close();

    // Save parsing result to syntax.txt
    ofstream syntaxFile("output/syntax.txt");
    syntaxFile << "PARSING:\n";
    parse(tokens, syntaxFile);
    syntaxFile.close();

    cout << "Outputs have been generated in the 'output' directory.\n";

    return 0;
}