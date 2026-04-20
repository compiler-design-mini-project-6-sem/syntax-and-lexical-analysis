#include <iostream>
#include <vector>
#include "utils/token.h"

using namespace std;

vector<Token> tokenize(string filename);
void parse(vector<Token>& tokens);

int main() {
    vector<Token> tokens = tokenize("input/input.txt");

    cout << "TOKENS:\n";
    for (auto &t : tokens) {
        cout << t.value << " -> " << t.type << endl;
    }

    cout << "\nPARSING:\n";
    parse(tokens);

    return 0;
}