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
        if (t.type == "KEYWORD") symName = t.value + "_KW";
        else if (t.type == "IDENTIFIER") symName = "ID";
        else if (t.type == "NUMBER") symName = "NUM";
        else if (t.type == "STRING") symName = "STRING_LIT";
        else if (t.type == "ASSIGN_OP") symName = ":=";
        else if (t.type == "SEPARATOR") symName = t.value;

        st.push({symName, true, t.value});

        // Greedy Reduction Logic (Simplified SLR simulation)
        bool reduced = true;
        while (reduced) {
            reduced = false;
            
            // 1-Symbol Reductions
            if (st.size() >= 1) {
                Symbol top = st.top();
                if (top.name == "INTEGER_KW" || top.name == "REAL_KW" || top.name == "STRING_KW") {
                    st.pop(); st.push({"TYPE", false, ""});
                    out << "Reduced: TYPE -> " << top.name << endl;
                    reduced = true; continue;
                }
                if (top.name == "NUM" || top.name == "STRING_LIT") {
                    st.pop(); st.push({"VALUE", false, ""});
                    out << "Reduced: VALUE -> " << top.name << endl;
                    reduced = true; continue;
                }
            }

            // 2-Symbol Reductions
            if (st.size() >= 2) {
                Symbol top1 = st.top(); st.pop();
                Symbol top2 = st.top(); st.pop();

                if (top2.name == "PRINT_KW" && top1.name == "VALUE") {
                    st.push({"PRINT_STMT", false, ""});
                    out << "Reduced: PRINT_STMT -> PRINT VALUE" << endl;
                    reduced = true; continue;
                }
                
                if (top1.name == "DECL" || top1.name == "ASSIGN" || top1.name == "PRINT_STMT" || top1.name == "FOR_STMT") {
                    st.push(top2); st.push({"STMT", false, ""});
                    out << "Reduced: STMT -> " << top1.name << endl;
                    reduced = true; continue;
                }

                if ((top2.name == "TYPE" || top2.name == ",") && top1.name == "ID" && (i+1 == tokens.size() || tokens[i+1].value != ",")) {
                    st.push(top2); st.push({"ID_LIST", false, ""});
                    out << "Reduced: ID_LIST -> ID" << endl;
                    reduced = true; continue;
                }
                
                if (top2.name == "STMTS" && top1.name == "STMT") {
                    // Check if we should wait (e.g., if inside a loop that hasn't finished)
                    // In a simple simulation, we'll reduce if the next token is END or another statement start.
                    st.push({"STMTS", false, ""});
                    out << "Reduced: STMTS -> STMTS STMT" << endl;
                    reduced = true; continue;
                }
                
                if ((top2.name == "BEGIN_KW" || top2.name == "VALUE") && top1.name == "STMT") {
                    st.push(top2); st.push({"STMTS", false, ""});
                    out << "Reduced: STMTS -> STMT" << endl;
                    reduced = true; continue;
                }

                st.push(top2); st.push(top1);
            }

            // 3-Symbol Reductions
            if (st.size() >= 3) {
                Symbol top1 = st.top(); st.pop();
                Symbol top2 = st.top(); st.pop();
                Symbol top3 = st.top(); st.pop();

                if (top3.name == "ID" && top2.name == ":=" && top1.name == "VALUE") {
                    st.push({"ASSIGN", false, ""});
                    out << "Reduced: ASSIGN -> ID := VALUE" << endl;
                    reduced = true; continue;
                }

                if (top3.name == "TYPE" && top2.name == "ID_LIST") {
                     st.push({"DECL", false, ""});
                     st.push(top1); // Put back the lookahead
                     out << "Reduced: DECL -> TYPE ID_LIST" << endl;
                     reduced = true; continue;
                }

                if (top3.name == "ID" && top2.name == "," && top1.name == "ID_LIST") {
                    st.push({"ID_LIST", false, ""});
                    out << "Reduced: ID_LIST -> ID , ID_LIST" << endl;
                    reduced = true; continue;
                }

                if (top3.name == "BEGIN_KW" && top2.name == "STMTS" && top1.name == "END_KW") {
                    st.push({"P", false, ""});
                    out << "Reduced: P -> BEGIN STMTS END" << endl;
                    reduced = true; continue;
                }

                st.push(top3); st.push(top2); st.push(top1);
            }
            
            // 7-Symbol Reductions (FOR loop)
            if (st.size() >= 7) {
                vector<Symbol> top7(7);
                for(int j=6; j>=0; --j) { top7[j] = st.top(); st.pop(); }

                if (top7[0].name == "FOR_KW" && top7[1].name == "ID" && top7[2].name == ":=" && top7[3].name == "VALUE" && top7[4].name == "TO_KW" && top7[5].name == "VALUE" && top7[6].name == "STMTS") {
                    st.push({"FOR_STMT", false, ""});
                    out << "Reduced: FOR_STMT -> FOR ID := VALUE TO VALUE STMTS" << endl;
                    reduced = true; continue;
                }
                
                for(int j=0; j<7; ++j) st.push(top7[j]);
            }
        }
    }




    if (!st.empty() && st.top().name == "P") {
        out << "Parsing Completed Successfully\n";
    } else {
        out << "Parsing Failed: Stack not reduced to start symbol.\n";
    }
}