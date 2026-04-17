#include <iostream>
#include <stack>
#include <fstream>
#include <vector>
#include <string>
#include "../utils/token.h"
using namespace std;

void parse(vector<Token>& tokens) {

    stack<string> st;
    ofstream out("output/syntax_output.txt");

    st.push("START");

    for (auto &t : tokens) {

        if (t.type == "KEYWORD" && t.value == "BEGIN") {
            st.push("BEGIN");
        }
        else if (t.type == "KEYWORD" && t.value == "END") {
            if (!st.empty()) st.pop();
        }
        else if (t.type == "IDENTIFIER") {
            st.push("ID");
        }
        else if (t.type == "NUMBER") {
            st.push("NUM");
        }
        else if (t.type == "STRING") {
            st.push("STRING");
        }
        else if (t.type == "ASSIGN_OP") {
            st.push(":=");
        }

        // Simple reduction example
        if (st.size() >= 3) {
            string top1 = st.top(); st.pop();
            string top2 = st.top(); st.pop();
            string top3 = st.top(); st.pop();

            if (top3 == "ID" && top2 == ":=" && (top1 == "NUM" || top1 == "STRING")) {
                st.push("ASSIGN");
                out << "Reduced: ID := VALUE\n";
            } else {
                st.push(top3);
                st.push(top2);
                st.push(top1);
            }
        }
    }

    out << "Parsing Completed Successfully\n";
}