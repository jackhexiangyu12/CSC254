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
#include <map>
#include <list>
#include <algorithm>

#include "scan.h"

using namespace std;

const char* names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof",
                       "if", "while", "end", "eqeq", "neq", "gt", "st", "gte", "ste"};

static token input_token;

map<string,list<token>> FIRST;
map<string,list<token>> FOLLOW;
map<string, bool> EPS;

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
    cout << "report  syntax error in default case" << endl;
    exit (1);
}

void report_error (string symbol) {
    cout << "report  syntax error in " << symbol << endl;
    return;
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

void match (token expected) {
    if (input_token == expected) {
        cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            cout << ": " << token_image;
        cout << endl;
        input_token = scan ();
    }
    else report_error ("match");
}

void program ();
void stmt_list ();
void stmt ();
void cond ();
void expr ();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void cond_op ();
void add_op ();
void mul_op ();

void program () {
    check_for_errors ("P");
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof" << endl;
            stmt_list ();
            match (t_eof);
            break;
        default: error ();
    }
}

void stmt_list () {
    check_for_errors ("SL");
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            cout << "predict stmt_list --> stmt stmt_list" << endl;
            stmt ();
            stmt_list ();
            break;
        case t_end:
        case t_eof:
            cout << "predict stmt_list --> epsilon" << endl;
            break;          /*  epsilon production */
        default: error ();
    }
}

void stmt () {
    check_for_errors ("S");
    switch (input_token) {
        case t_id:
            cout << "predict stmt --> id gets expr" << endl;
            match (t_id);
            match (t_gets);
            expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id" << endl;
            match (t_read);
            match (t_id);
            break;
        case t_write:
            cout << "predict stmt --> write expr" << endl;
            match (t_write);
            expr ();
            break;
        case t_if:
            cout << "predict stmt --> if cond stmt_list end" << endl;
            match (t_if);
            cond ();
            stmt_list ();
            match (t_end);
            break;
        case t_while:
            cout << "predict stmt --> while cond stmt_list end" << endl;
            match (t_while);
            cond ();
            stmt_list ();
            match (t_end);
            break;
        default: error ();
    }
}

void cond () {
    check_for_errors ("C");
    switch (input_token) {
        case t_lparen:
        case t_id:
        case t_literal:
            cout << "predict cond -> expr cond_op expr" << endl;
            expr ();
            cond_op ();
            expr ();
            break;
        default: error ();
    }
}

void expr () {
    check_for_errors ("E");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail" << endl;
            term ();
            term_tail ();
            break;
        default: error ();
    }
}

void term_tail () {
    check_for_errors ("TT");
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail" << endl;
            add_op ();
            term ();
            term_tail ();
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
            break;          /*  epsilon production */
        default: error ();
    }
}

void term () {
    check_for_errors ("T");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail" << endl;
            factor ();
            factor_tail ();
            break;
        default: error ();
    }
}

void factor_tail () {
    check_for_errors ("FT");
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail" << endl;
            mul_op ();
            factor ();
            factor_tail ();
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
            break;          /*  epsilon production */
        default: error ();
    }
}

void factor () {
    check_for_errors ("F");
    switch (input_token) {
        case t_id :
            cout << "predict factor --> id" << endl;
            match (t_id);
            break;
        case t_literal:
            cout << "predict factor --> literal" << endl;
            match (t_literal);
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen" << endl;
            match (t_lparen);
            expr ();
            match (t_rparen);
            break;
        default: error ();
    }
}

void cond_op () {
    check_for_errors ("ro");
    switch (input_token) {
        case t_eqeq:
            cout << "predict cond_op --> eqeq" << endl;
            match (t_eqeq);
            break;
        case t_neq:
            cout << "predict cond_op --> neq" << endl;
            match (t_neq);
            break;
        case t_gt:
            cout << "predict cond_op --> gt" << endl;
            match (t_gt);
            break;
        case t_st:
            cout << "predict cond_op --> st" << endl;
            match (t_st);
            break;
        case t_gte:
            cout << "predict cond_op --> gte" << endl;
            match (t_gte);
            break;
        case t_ste:
            cout << "predict cond_op --> ste" << endl;
            match (t_ste);
            break;
    }
}

void add_op () {
    check_for_errors ("ao");
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add" << endl;
            match (t_add);
            break;
        case t_sub:
            cout << "predict add_op --> sub" << endl;
            match (t_sub);
            break;
        default: error ();
    }
}

void mul_op () {
    check_for_errors ("mo");
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul" << endl;
            match (t_mul);
            break;
        case t_div:
            cout << "predict mul_op --> div" << endl;
            match (t_div);
            break;
        default: error ();
    }
}

int main () {
    init_first ();
    init_follow ();
    init_eps ();

    input_token = scan ();
    program ();
    return 0;
}
