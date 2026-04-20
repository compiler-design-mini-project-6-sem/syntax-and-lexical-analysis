#include <iostream>
#include <stack>
#include <fstream>
#include <vector>
#include <string>
#include "../utils/token.h"
using namespace std;

// Representation of grammar symbols (Terminals and Non-Terminals)
struct Symbol {
    string name;
    bool isTerminal;
    string value;
};

void parse(vector<Token>& tokens) {
    stack<Symbol> st;
    ofstream out("output/syntax_output.txt");

    auto reduce = [&](int n, string lhs) {
        vector<Symbol> rhs;
        for (int i = 0; i < n; ++i) {
            if (st.empty()) return false;
            rhs.insert(rhs.begin(), st.top());
            st.pop();
        }
        st.push({lhs, false, ""});
        out << "Reduced: " << lhs << " -> ";
        for (auto &s : rhs) out << s.name << " ";
        out << endl;
        return true;
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        Token t = tokens[i];
        
        // Push terminal onto stack (SHIFT)
        string symName = t.type;
        if (t.type == "KEYWORD") symName = t.value;
        else if (t.type == "IDENTIFIER") symName = "ID";
        else if (t.type == "NUMBER") symName = "NUM";
        else if (t.type == "ASSIGN_OP") symName = ":=";
        else if (t.type == "SEPARATOR") symName = t.value;

        st.push({symName, true, t.value});

        // Greedy Reduction Logic (Simplified SLR simulation)
        bool reduced = true;
        while (reduced) {
            reduced = false;
            if (st.size() >= 1) {
                Symbol top = st.top();
                
                // TYPE -> INTEGER | REAL | STRING
                if (top.name == "INTEGER" || top.name == "REAL" || top.name == "STRING") {
                    st.pop();
                    st.push({"TYPE", false, ""});
                    out << "Reduced: TYPE -> " << top.name << endl;
                    reduced = true; continue;
                }
                
                // VALUE -> NUM | STRING
                if (top.name == "NUM" || top.name == "STRING") {
                    st.pop();
                    st.push({"VALUE", false, ""});
                    out << "Reduced: VALUE -> " << top.name << endl;
                    reduced = true; continue;
                }
            }

            if (st.size() >= 2) {
                // Peek top 2
                Symbol top1 = st.top(); st.pop();
                Symbol top2 = st.top(); st.pop();

                // PRINT_STMT -> PRINT STRING
                if (top2.name == "PRINT" && top1.name == "VALUE") {
                    st.push({"PRINT_STMT", false, ""});
                    out << "Reduced: PRINT_STMT -> PRINT VALUE" << endl;
                    reduced = true; continue;
                }
                
                // STMT -> DECL | ASSIGN | PRINT_STMT | FOR_STMT
                if (top1.name == "DECL" || top1.name == "ASSIGN" || top1.name == "PRINT_STMT" || top1.name == "FOR_STMT") {
                    st.push(top2); // put back
                    st.push({"STMT", false, ""});
                    out << "Reduced: STMT -> " << top1.name << endl;
                    reduced = true; continue;
                }

                // ID_LIST -> ID
                if (top2.name == "TYPE" && top1.name == "ID") {
                    st.push(top2);
                    st.push({"ID_LIST", false, ""});
                    out << "Reduced: ID_LIST -> ID" << endl;
                    reduced = true; continue;
                }
                
                // STMTS -> STMTS STMT
                if (top2.name == "STMTS" && top1.name == "STMT") {
                    st.push({"STMTS", false, ""});
                    out << "Reduced: STMTS -> STMTS STMT" << endl;
                    reduced = true; continue;
                }
                
                // STMTS -> STMT (base case)
                if (top2.name == "BEGIN" && top1.name == "STMT") {
                    st.push(top2);
                    st.push({"STMTS", false, ""});
                    out << "Reduced: STMTS -> STMT" << endl;
                    reduced = true; continue;
                }

                st.push(top2);
                st.push(top1);
            }

            if (st.size() >= 3) {
                Symbol s1 = st.top(); st.pop();
                Symbol s2 = st.top(); st.pop();
                Symbol s3 = st.top(); st.pop();

                // ASSIGN -> ID := VALUE
                if (s3.name == "ID" && s2.name == ":=" && s1.name == "VALUE") {
                    st.push({"ASSIGN", false, ""});
                    out << "Reduced: ASSIGN -> ID := VALUE" << endl;
                    reduced = true; continue;
                }

                // DECL -> TYPE ID_LIST
                if (s3.name == "TYPE" && s2.name == "ID_LIST" && (i+1 == tokens.size() || tokens[i+1].value != ",")) {
                    // This is tricky because ID_LIST is recursive. 
                    // Let's handle it by checking if next token is NOT a comma.
                    st.push({"DECL", false, ""});
                    out << "Reduced: DECL -> TYPE ID_LIST" << endl;
                    reduced = true; continue;
                }
                
                // ID_LIST -> ID , ID_LIST
                if (s3.name == "ID" && s2.name == "," && s1.name == "ID_LIST") {
                    st.push({"ID_LIST", false, ""});
                    out << "Reduced: ID_LIST -> ID , ID_LIST" << endl;
                    reduced = true; continue;
                }

                // P -> BEGIN STMTS END
                if (s3.name == "BEGIN" && s2.name == "STMTS" && s1.name == "END") {
                    st.push({"P", false, ""});
                    out << "Reduced: P -> BEGIN STMTS END" << endl;
                    reduced = true; continue;
                }

                st.push(s3);
                st.push(s2);
                st.push(s1);
            }
            
            // FOR_STMT -> FOR ID := NUM TO NUM STMTS
            if (st.size() >= 7) {
                Symbol s1 = st.top(); st.pop();
                Symbol s2 = st.top(); st.pop();
                Symbol s3 = st.top(); st.pop();
                Symbol s4 = st.top(); st.pop();
                Symbol s5 = st.top(); st.pop();
                Symbol s6 = st.top(); st.pop();
                Symbol s7 = st.top(); st.pop();

                if (s7.name == "FOR" && s6.name == "ID" && s5.name == ":=" && (s4.name == "NUM" || s4.name == "VALUE") && s3.name == "TO" && (s2.name == "NUM" || s2.name == "VALUE") && (s1.name == "STMTS" || s1.name == "STMT")) {
                    st.push({"FOR_STMT", false, ""});
                    out << "Reduced: FOR_STMT -> FOR ID := VALUE TO VALUE STMTS" << endl;
                    reduced = true; continue;
                }
                
                st.push(s7); st.push(s6); st.push(s5); st.push(s4); st.push(s3); st.push(s2); st.push(s1);
            }
        }
    }

    if (!st.empty() && st.top().name == "P") {
        out << "Parsing Completed Successfully\n";
    } else {
        out << "Parsing Failed: Stack not reduced to start symbol.\n";
    }
}