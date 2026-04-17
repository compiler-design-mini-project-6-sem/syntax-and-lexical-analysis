#include <set>
#include <fstream>
#include <vector>
#include <string>
#include "../utils/token.h"
using namespace std;

set<string> keywords = {
    "BEGIN","END","PRINT","INTEGER","REAL","STRING","FOR","TO"
};

bool isKeyword(string word) {
    return keywords.count(word);
}

vector<Token> tokenize(string filename) {
    ifstream file(filename);
    vector<Token> tokens;
    string word;

    while (file >> word) {

        if (isKeyword(word)) {
            tokens.push_back({"KEYWORD", word});
        }
        else if (word == ":=") {
            tokens.push_back({"ASSIGN_OP", word});
        }
        else if (isdigit(word[0]) || (word[0]=='-' && word.size()>1)) {
            tokens.push_back({"NUMBER", word});
        }
        else if (word[0] == '"') {
            tokens.push_back({"STRING", word});
        }
        else if (word == "," || word == "[" || word == "]") {
            tokens.push_back({"SEPARATOR", word});
        }
        else {
            tokens.push_back({"IDENTIFIER", word});
        }
    }

    return tokens;
}

void saveTokens(vector<Token>& tokens) {
    ofstream out("lexer/tokens.txt");
    ofstream out2("output/lexical_output.txt");

    for (auto &t : tokens) {
        string line = t.value + " -> " + t.type;
        out << line << endl;
        out2 << line << endl;
    }
}