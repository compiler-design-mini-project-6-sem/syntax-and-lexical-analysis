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
    char c;

    while (file.get(c)) {
        if (isspace(c)) continue;

        if (isalpha(c)) {
            string word = "";
            word += c;
            while (file.get(c)) {
                if (isalnum(c) || c == '_') {
                    word += c;
                } else {
                    file.unget();
                    break;
                }
            }
            if (isKeyword(word)) {
                tokens.push_back({"KEYWORD", word});
            } else {
                tokens.push_back({"IDENTIFIER", word});
            }
        }
        else if (isdigit(c) || (c == '-' && isdigit(file.peek()))) {
            string num = "";
            num += c;
            while (file.get(c)) {
                if (isdigit(c) || c == '.' || c == 'E' || c == 'e') {
                    num += c;
                }
                else if ((c == '-' || c == '+') && (num.back() == 'E' || num.back() == 'e')) {
                    num += c;
                }
                else {
                    file.unget();
                    break;
                }
            }
            tokens.push_back({"NUMBER", num});
        }
        else if (c == '"') {
            string str = "\"";
            while (file.get(c)) {
                str += c;
                if (c == '"') break;
            }
            tokens.push_back({"STRING", str});
        }
        else if (c == ':') {
            if (file.peek() == '=') {
                file.get(c);
                tokens.push_back({"ASSIGN_OP", ":="});
            } else {
                tokens.push_back({"UNKNOWN", ":"});
            }
        }
        else if (c == ',' || c == '[' || c == ']' || c == '.') {
            string sep = "";
            sep += c;
            tokens.push_back({"SEPARATOR", sep});
        }
        else {
            string unk = "";
            unk += c;
            tokens.push_back({"UNKNOWN", unk});
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