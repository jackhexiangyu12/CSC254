/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2019.
*/

// #include "stdio.h"
// #include "stdlib.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "scan.h"

using namespace std;

const char* names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof"
                       "if", "while", "end", "eqeq", "neq", "gt", "st", "gte", "ste"};

static token input_token;

void error () {
    cout << "syntax error" << endl;
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        cout << "matched " << names[input_token] << endl;
        if (input_token == t_id || input_token == t_literal)
            cout << ": " << token_image << endl;
        cout << endl;
        input_token = scan ();
    }
    else error ();
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
        case t_eof:
            cout << "predict stmt_list --> epsilon" << endl;
            break;          /*  epsilon production */
        default: error ();
    }
}

void stmt () {
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
            cout << "predict term_tail --> epsilon" << endl;
            break;          /*  epsilon production */
        default: error ();
    }
}

void term () {
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
            cout << "predict factor_tail --> epsilon" << endl;
            break;          /*  epsilon production */
        default: error ();
    }
}

void factor () {
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
    input_token = scan ();
    program ();
    return 0;
}