/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2019.
*/

// #include "stdio.h"
// #include "stdlib.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <map>
#include <list>
#include <stack>
#include <algorithm>

#include "scan.h"

using namespace std;

const char* names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof",
                       "if", "while", "end", "eqeq", "neq", "gt", "st", "gte", "ste"};

static token input_token;

// FIRST FOLLOW and EPS for recovery
map<string,list<token>> FIRST;
map<string,list<token>> FOLLOW;
map<string, bool> EPS;
// Stack for syntax tree generation
stack<string> S;

// Convert current token to string
string return_token () {
    string res = "";
    switch (input_token) {
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            res = names[input_token];
            break;
        case t_id:
            res = "(id \"" + string(token_image) + "\")";
            break;
        case t_literal:
            res = "(num \"" + string(token_image) + "\")";
            break;
        case t_gets:
            res = ":=";
            break;
        case t_add:
            res = "+";
            break;
        case t_sub:
            res = "-";
            break;
        case t_mul:
            res = "*";
            break;
        case t_div:
            res = "/";
            break;
        case t_lparen:
            res = "(";
            break;
        case t_rparen:
            res = ")";
            break;
        case t_eqeq:
            res = "==";
            break;
        case t_neq:
            res = "!=";
            break;
        case t_gt:
            res = ">";
            break;
        case t_st:
            res = "<";
            break;
        case t_gte:
            res = ">=";
            break;
        case t_ste:
            res = "<=";
            break;
        case t_end:
        case t_eof:
            res = "";
            break;
        default:
            res = "error";
            break;
    }

    return res;
}

// Init FIRST
void init_first () {
    list<token> first_p = {t_id, t_read, t_write, t_if, t_while, t_eof};
    list<token> first_sl = {t_id, t_read, t_write, t_if, t_while};
    list<token> first_s = {t_id, t_read, t_write, t_if, t_while};
    list<token> first_c = {t_lparen, t_id, t_literal};
    list<token> first_e = {t_lparen, t_id, t_literal};
    list<token> first_tt = {t_add, t_sub};
    list<token> first_t = {t_lparen, t_id, t_literal};
    list<token> first_ft = {t_mul, t_div};
    list<token> first_f = {t_lparen, t_id, t_literal};
    list<token> first_ro = {t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste};
    list<token> first_ao = {t_add, t_sub};
    list<token> first_mo = {t_mul, t_div};

    FIRST.insert({"P", first_p});
    FIRST.insert({"SL", first_sl});
    FIRST.insert({"S", first_s});
    FIRST.insert({"C", first_c});
    FIRST.insert({"E", first_e});
    FIRST.insert({"TT", first_tt});
    FIRST.insert({"T", first_t});
    FIRST.insert({"FT", first_ft});
    FIRST.insert({"F", first_f});
    FIRST.insert({"ro", first_ro});
    FIRST.insert({"ao", first_ao});
    FIRST.insert({"mo", first_mo});
}

// Init FOLLOW
void init_follow () {
    list<token> follow_p = {};
    list<token> follow_sl = {t_end, t_eof};
    list<token> follow_s = {t_id, t_read, t_write, t_if, t_while, t_end, t_eof};
    list<token> follow_c = {t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_e = {t_rparen, t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste, t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_tt = {t_rparen, t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste, t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_t = {t_add, t_sub, t_rparen, t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste, t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_ft = {t_add, t_sub, t_rparen, t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste, t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_f = {t_mul, t_div, t_add, t_sub, t_rparen, t_eqeq, t_neq, t_gt, t_st, t_gte, t_ste, t_id, t_read, t_write, t_if, t_while, t_end};
    list<token> follow_ro = {t_lparen, t_id, t_literal};
    list<token> follow_ao = {t_lparen, t_id, t_literal};
    list<token> follow_mo = {t_lparen, t_id, t_literal};

    FOLLOW.insert({"P", follow_p});
    FOLLOW.insert({"SL", follow_sl});
    FOLLOW.insert({"S", follow_s});
    FOLLOW.insert({"C", follow_c});
    FOLLOW.insert({"E", follow_e});
    FOLLOW.insert({"TT", follow_tt});
    FOLLOW.insert({"T", follow_t});
    FOLLOW.insert({"FT", follow_ft});
    FOLLOW.insert({"F", follow_f});
    FOLLOW.insert({"ro", follow_ro});
    FOLLOW.insert({"ao", follow_ao});
    FOLLOW.insert({"mo", follow_mo});
}

// Init EPS
void init_eps () {
    EPS.insert({"P", false});
    EPS.insert({"SL", true});
    EPS.insert({"S", false});
    EPS.insert({"C", false});
    EPS.insert({"E", false});
    EPS.insert({"TT", true});
    EPS.insert({"T", false});
    EPS.insert({"FT", true});
    EPS.insert({"F", false});
    EPS.insert({"ro", false});
    EPS.insert({"ao", false});
    EPS.insert({"mo", false});
}

void error () {
    cout << "syntax error" << endl;
    exit (1);
}

void report_error (string symbol) {
    cout << "report  syntax error in " << symbol << endl;
}

void check_for_errors (string symbol) {
    list<token> first_curr = FIRST[symbol];
    list<token> follow_curr = FOLLOW[symbol];
    bool eps_curr = EPS[symbol];

    if (!(find(first_curr.begin(), first_curr.end(), input_token) != first_curr.end() || eps_curr)) {
        report_error(symbol);
        do {
            input_token = scan();
        } while (!(
            find(first_curr.begin(), first_curr.end(), input_token) != first_curr.end()
            || find(follow_curr.begin(), follow_curr.end(), input_token) != follow_curr.end()
            || input_token == t_eof
        ));
    }
}

string match (token expected) {
    string res = "";
    if (input_token == expected) {
        res += return_token ();

        cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            cout << ": " << token_image;
        cout << endl;
        input_token = scan ();
    }
    else report_error ("match");

    return res;
}

string program ();
string stmt_list ();
string stmt ();
string cond ();
string expr ();
string term_tail ();
string term ();
string factor_tail ();
string factor ();
string cond_op ();
string add_op ();
string mul_op ();

string program () {
    check_for_errors ("P");

    string res = "(program ";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof" << endl;
            res += stmt_list ();
            res += match (t_eof);
            break;
        default: report_error ("default case");
    }
    res += ")";

    return res;
}

string stmt_list () {
    check_for_errors ("SL");

    string res = "";
    string tmp = "";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            cout << "predict stmt_list --> stmt stmt_list" << endl;
            res += "[";
            res += stmt ();
            tmp += stmt_list ();
            if (tmp.compare("]") != 0) res = res + " " + tmp;
            else res = res + tmp;
            break;
        case t_end:
        case t_eof:
            cout << "predict stmt_list --> epsilon" << endl;
            res += "]";
            break;          /*  epsilon production */
        default: report_error ("default case");
    }

    return res;
}

string stmt () {
    check_for_errors ("S");

    string res = "(";
    string tmp = "";
    switch (input_token) {
        case t_id:
            cout << "predict stmt --> id gets expr" << endl;
            tmp += match (t_id);
            res += match (t_gets);
            res += " ";
            res += tmp;
            res += " ";
            res += expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id" << endl;
            res += match (t_read);
            res += " ";
            res += match (t_id);
            break;
        case t_write:
            cout << "predict stmt --> write expr" << endl;
            res += match (t_write);
            res += " ";
            res += expr ();
            break;
        case t_if:
            cout << "predict stmt --> if cond stmt_list end" << endl;
            res += match (t_if);
            res += " ";
            res += cond ();
            res += " ";
            res += stmt_list ();
            res += match (t_end);
            break;
        case t_while:
            cout << "predict stmt --> while cond stmt_list end" << endl;
            res += match (t_while);
            res += " ";
            res += cond ();
            res += " ";
            res += stmt_list ();
            res += match (t_end);
            break;
        default: report_error ("default case");
    }
    res += ")";

    return res;
}

string cond () {
    check_for_errors ("C");

    string res = "";
    string tmp = "";
    switch (input_token) {
        case t_lparen:
        case t_id:
        case t_literal:
            cout << "predict cond -> expr cond_op expr" << endl;
            tmp += expr ();
            res += cond_op ();
            res += " ";
            res += tmp;
            res += " ";
            res += expr ();
            break;
        default: report_error ("default case");
    }

    return res;
}

string expr () {
    check_for_errors ("E");

    string res = "";
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail" << endl;
            S.push (term ());
            res += term_tail ();
            break;
        default: report_error ("default case");
    }

    return res;
}

string term_tail () {
    check_for_errors ("TT");

    string res = "";
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail" << endl;
            res += "(";
            res += add_op ();
            res += " ";
            res += S.top ();
            S.pop ();
            res += " ";
            S.push (term ());
            res += term_tail ();
            res += ")";
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        case t_if:
        case t_while:
        case t_end:
        case t_eqeq:
        case t_neq:
        case t_gt:
        case t_st:
        case t_gte:
        case t_ste:
            cout << "predict term_tail --> epsilon" << endl;
            res += S.top ();
            S.pop ();
            break;          /*  epsilon production */
        default: report_error ("default case");
    }

    return res;
}

string term () {
    check_for_errors ("T");

    string res = "";
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail" << endl;
            S.push (factor ());
            res += factor_tail ();
            break;
        default: report_error ("default case");
    }

    return res;
}

string factor_tail () {
    check_for_errors ("FT");

    string res = "";
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail" << endl;
            res += "(";
            res += mul_op ();
            res += " ";
            res += S.top ();
            S.pop ();
            res += " ";
            S.push (factor ());
            res += factor_tail ();
            res += ")";
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        case t_if:
        case t_while:
        case t_end:
        case t_eqeq:
        case t_neq:
        case t_gt:
        case t_st:
        case t_gte:
        case t_ste:
            cout << "predict factor_tail --> epsilon" << endl;
            res += S.top ();
            S.pop ();
            break;          /*  epsilon production */
        default: report_error ("default case");
    }

    return res;
}

string factor () {
    check_for_errors ("F");

    string res = "";
    switch (input_token) {
        case t_id :
            cout << "predict factor --> id" << endl;
            res += match (t_id);
            break;
        case t_literal:
            cout << "predict factor --> literal" << endl;
            res += match (t_literal);
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen" << endl;
            res += match (t_lparen);
            res += expr ();
            res += match (t_rparen);
            break;
        default: report_error ("default case");
    }

    return res;
}

string cond_op () {
    check_for_errors ("ro");

    string res ="";
    switch (input_token) {
        case t_eqeq:
            cout << "predict cond_op --> eqeq" << endl;
            res += match (t_eqeq);
            break;
        case t_neq:
            cout << "predict cond_op --> neq" << endl;
            res += match (t_neq);
            break;
        case t_gt:
            cout << "predict cond_op --> gt" << endl;
            res += match (t_gt);
            break;
        case t_st:
            cout << "predict cond_op --> st" << endl;
            res += match (t_st);
            break;
        case t_gte:
            cout << "predict cond_op --> gte" << endl;
            res += match (t_gte);
            break;
        case t_ste:
            cout << "predict cond_op --> ste" << endl;
            res += match (t_ste);
            break;
        default: report_error ("default case");
    }

    return res;
}

string add_op () {
    check_for_errors ("ao");

    string res = "";
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add" << endl;
            res += match (t_add);
            break;
        case t_sub:
            cout << "predict add_op --> sub" << endl;
            res += match (t_sub);
            break;
        default: report_error ("default case");
    }

    return res;
}

string mul_op () {
    check_for_errors ("mo");

    string res = "";
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul" << endl;
            res += match (t_mul);
            break;
        case t_div:
            cout << "predict mul_op --> div" << endl;
            res += match (t_div);
            break;
        default: report_error ("default case");
    }

    return res;
}

int main () {
    init_first ();
    init_follow ();
    init_eps ();

    input_token = scan ();
    cout << program () << endl;

    return 0;
}
