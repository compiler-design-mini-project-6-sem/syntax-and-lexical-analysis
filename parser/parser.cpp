#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include "../utils/token.h"

using namespace std;

string mapToken(Token t) {
    if (t.type == "KEYWORD") return t.value + "_KW";
    if (t.type == "IDENTIFIER") return "ID";
    if (t.type == "NUMBER") return "NUM";
    if (t.type == "STRING") return "STRING";
    if (t.type == "ASSIGN_OP") return ":=";
    return t.value;
}

void parse(vector<Token>& tokens) {
    stack<string> st;

    for (size_t i = 0; i < tokens.size(); i++) {
        st.push(mapToken(tokens[i]));

        bool reduced = true;
        while (reduced) {
            reduced = false;

            // VALUE
            if (!st.empty() && (st.top() == "NUM" || st.top() == "STRING")) {
                st.pop(); st.push("VALUE");
                reduced = true;
                continue;
            }

            // TYPE
            if (!st.empty() &&
                (st.top() == "INTEGER_KW" || st.top() == "REAL_KW" || st.top() == "STRING_KW")) {
                st.pop(); st.push("TYPE");
                reduced = true;
                continue;
            }

            // ASSIGN → ID := VALUE
            if (st.size() >= 3) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();
                string c = st.top(); st.pop();

                if (c == "ID" && b == ":=" && a == "VALUE") {
                    st.push("ASSIGN");
                    reduced = true;
                    continue;
                } else {
                    st.push(c); st.push(b); st.push(a);
                }
            }

            // ID_LIST → ID , ID_LIST
            if (st.size() >= 3) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();
                string c = st.top(); st.pop();

                if (c == "ID" && b == "," && a == "ID_LIST") {
                    st.push("ID_LIST");
                    reduced = true;
                    continue;
                } else {
                    st.push(c); st.push(b); st.push(a);
                }
            }

            // ID_LIST → ID  (ONLY when safe)
            if (!st.empty() && st.top() == "ID") {
                st.pop();
                st.push("ID_LIST");
                reduced = true;
                continue;
            }

            // DECL → TYPE ID_LIST
            if (st.size() >= 2) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();

                if (b == "TYPE" && a == "ID_LIST") {
                    st.push("DECL");
                    reduced = true;
                    continue;
                } else {
                    st.push(b); st.push(a);
                }
            }

            // PRINT → PRINT VALUE
            if (st.size() >= 2) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();

                if (b == "PRINT_KW" && a == "VALUE") {
                    st.push("PRINT_STMT");
                    reduced = true;
                    continue;
                } else {
                    st.push(b); st.push(a);
                }
            }

            // STMT
            if (!st.empty()) {
                string top = st.top();
                if (top == "DECL" || top == "ASSIGN" || top == "PRINT_STMT" || top == "FOR_STMT") {
                    st.pop();
                    st.push("STMT");
                    reduced = true;
                    continue;
                }
            }

            // STMTS → STMTS STMT
            if (st.size() >= 2) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();

                if (b == "STMTS" && a == "STMT") {
                    st.push("STMTS");
                    reduced = true;
                    continue;
                } else {
                    st.push(b); st.push(a);
                }
            }

            // STMTS → STMT (only when next is END or finished)
            if (!st.empty() && st.top() == "STMT") {
                st.pop();
                st.push("STMTS");
                reduced = true;
                continue;
            }

            // FOR loop (must come AFTER STMTS is formed)
            if (st.size() >= 8) {
                vector<string> v(8);
                for (int j = 7; j >= 0; j--) {
                    v[j] = st.top(); st.pop();
                }

                if (v[0] == "FOR_KW" && v[1] == "ID" && v[2] == ":=" &&
                    v[3] == "VALUE" && v[4] == "TO_KW" &&
                    v[5] == "VALUE" && v[6] == "STMTS" &&
                    v[7] == "END_KW") {

                    st.push("FOR_STMT");
                    reduced = true;
                    continue;
                } else {
                    for (int j = 0; j < 8; j++) st.push(v[j]);
                }
            }

            // PROGRAM
            if (st.size() >= 3) {
                string a = st.top(); st.pop();
                string b = st.top(); st.pop();
                string c = st.top(); st.pop();

                if (c == "BEGIN_KW" && b == "STMTS" && a == "END_KW") {
                    st.push("P");
                    reduced = true;
                    continue;
                } else {
                    st.push(c); st.push(b); st.push(a);
                }
            }
        }
    }

    if (!st.empty() && st.top() == "P")
        cout << "Parsing Successful\n";
    else
        cout << "Parsing Failed\n";
}