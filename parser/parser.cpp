#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include "../utils/token.h"

using namespace std;

string getSymbol(Token t) {
    if (t.type=="KEYWORD") return t.value;
    if (t.type=="IDENTIFIER") return "id";
    if (t.type=="NUMBER") return "num";
    if (t.type=="STRING") return "str";
    if (t.type=="ASSIGN_OP") return ":=";
    return t.value;
}

void parse(vector<Token>& tokens, ostream& out) {
    vector<string> input;
    for (auto &t: tokens) input.push_back(getSymbol(t));
    input.push_back("$");

    stack<string> st;
    st.push("$");
    st.push("program");

    int i=0;

    while (!st.empty()) {
        string top = st.top();
        string curr = input[i];

        // LOGGING THE PARSING STEP
        out << "Stack: ";
        stack<string> tempSt = st;
        vector<string> stackItems;
        while (!tempSt.empty()) { stackItems.push_back(tempSt.top()); tempSt.pop(); }
        for (int j = stackItems.size() - 1; j >= 0; j--) out << stackItems[j] << " ";
        out << " | Input: " << curr << endl;

        if (top == curr) {
            out << "  [MATCH] " << curr << endl;
            st.pop();
            i++;
        }

        else if (top=="program") {
            out << "  [EXPAND] program -> BEGIN stmt_list END" << endl;
            st.pop();
            st.push("END");
            st.push("stmt_list");
            st.push("BEGIN");
        }

        else if (top=="stmt_list") {
            st.pop();
            if (curr=="PRINT" || curr=="INTEGER" || curr=="REAL" || curr=="STRING" || curr=="FOR" || curr=="id") {
                out << "  [EXPAND] stmt_list -> stmt stmt_list" << endl;
                st.push("stmt_list");
                st.push("stmt");
            } else {
                out << "  [EXPAND] stmt_list -> epsilon" << endl;
            }
        }

        else if (top=="stmt") {
            st.pop();
            if (curr=="PRINT") {
                out << "  [EXPAND] stmt -> PRINT expr ;" << endl;
                st.push(";");
                st.push("expr");
                st.push("PRINT");
            }
            else if (curr=="INTEGER" || curr=="REAL" || curr=="STRING") {
                out << "  [EXPAND] stmt -> type var_list ;" << endl;
                st.push(";");
                st.push("var_list");
                st.push("type");
            }
            else if (curr=="id") {
                out << "  [EXPAND] stmt -> id := expr ;" << endl;
                st.push(";");
                st.push("expr");
                st.push(":=");
                st.push("id");
            }
            else if (curr=="FOR") {
                out << "  [EXPAND] stmt -> FOR id := expr TO expr stmt_list END" << endl;
                st.push("END");
                st.push("stmt_list");
                st.push("expr");
                st.push("TO");
                st.push("expr");
                st.push(":=");
                st.push("id");
                st.push("FOR");
            }
        }

        else if (top=="type") {
            out << "  [EXPAND] type -> " << curr << endl;
            st.pop();
            st.push(curr);
        }

        else if (top=="var_list") {
            out << "  [EXPAND] var_list -> id var_list_tail" << endl;
            st.pop();
            st.push("var_list_tail");
            st.push("id");
        }

        else if (top=="var_list_tail") {
            st.pop();
            if (curr==",") {
                out << "  [EXPAND] var_list_tail -> , id var_list_tail" << endl;
                st.push("var_list_tail");
                st.push("id");
                st.push(",");
            } else {
                out << "  [EXPAND] var_list_tail -> epsilon" << endl;
            }
        }

        else if (top=="expr") {
            st.pop();
            if (curr=="id" || curr=="num" || curr=="str") {
                out << "  [EXPAND] expr -> " << curr << endl;
                st.push(curr);
            }
        }

        else {
            out << "Parsing Failed at token: " << curr << endl;
            return;
        }
    }

    out << "Parsing Successful\n";
}