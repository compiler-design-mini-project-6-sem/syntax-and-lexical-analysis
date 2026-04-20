#include <set>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
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
            string word;
            word += c;
            while (file.get(c)) {
                if (isalnum(c)) word += c;
                else { file.unget(); break; }
            }

            if (isKeyword(word))
                tokens.push_back({"KEYWORD", word});
            else
                tokens.push_back({"IDENTIFIER", word});
        }

        else if (isdigit(c) || (c=='-' && isdigit(file.peek()))) {
            string num;
            num += c;
            while (file.get(c)) {
                if (isdigit(c) || c=='.' || c=='E' || c=='e')
                    num += c;
                else if ((c=='+'||c=='-') && (num.back()=='E'||num.back()=='e'))
                    num += c;
                else { file.unget(); break; }
            }
            tokens.push_back({"NUMBER", num});
        }

        else if (c=='"') {
            string str="\"";
            while (file.get(c)) {
                str+=c;
                if (c=='"') break;
            }
            tokens.push_back({"STRING", str});
        }

        else if (c==':' && file.peek()=='=') {
            file.get(c);
            tokens.push_back({"ASSIGN_OP", ":="});
        }

        else if (c==',' || c==';') {
            tokens.push_back({"SYMBOL", string(1,c)});
        }
    }

    return tokens;
}