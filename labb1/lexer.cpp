//
// Created by Bashar Levin on 2024-01-31.
//

#include "lexer.h"
lexer::token lexer::get_current_token() const {
    return current_token;
}

void lexer::set_current_token(lexer::token token) {
    lexer::current_token = token;
}

lexer::token lexer::lookup(itr str, itr last) {
    if (str == last) { return END; }
    switch (*str) {
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '{':
            return LBRACKET;
        case '}':
            return RBRACKET;
        case '.':
            return WILDCARD;
        case '+':
            return EITHER_OP;
        case '*':
            return MANY_OP;
        case '\\':
            str++;
            if (*str == 'O') {
                str++;
                return LEVEL_OP;
            }
            if (*str == 'I') {
                str++;
                return IGNORE_OP;
            }
            break;
    }
    if ((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z'))
        return LETTER;
    if (*str >= '0' && *str <= '9') {
        return DIGIT;
    }
    throw std::invalid_argument("unified");
}

    void lexer::set_current_token(itr &str, itr &last) {
        set_current_token(lookup(str,last));
    }

    lexer::token lexer::get_current(itr &str, itr &last) {
        set_current_token(lookup(str,last));
    }
    lexer::token lexer::get_next(itr &str, itr &last) {
        return lookup(++str, last);
}
