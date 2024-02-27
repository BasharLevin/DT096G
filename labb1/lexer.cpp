//
// Created by Bashar Levin on 2024-01-31.
//

#include <string>
#include "lexer.h"
using it = std::string::iterator;


lexer::lexer(itr first, itr last) {
    while(first != last){
        input += *first;
        ++first;
    }
}


char lexer::check(it& first, it& last) {
    if (first == last) {
        return END;
    }

    switch (*first) {
        case '(':
            current_c = *first;
            return LPAREN;

        case ')':
            current_c = *first;
            return RPAREN;
        case '{':
            current_c = *first;
            return LBRACKET;
        case '}':
            current_c = *first;
            return RBRACKET;
        case '+':
            current_c = *first;
            return EITHER_OP;
        case '*':
            current_c = *first;
            return MANY_OP;
        case ' ':
            current_c = *first;
            return SPACE;
        case '.':
            current_c = *first;
            return DOT;
        case '\\':
        current_c = *first;
            return SLASH;

        default:
            if (isLetter(*first))
                return CHAR;
            if(isdigit(*first)){
                return DIGIT;
            }
    }
    return UNKOWN;
}


bool lexer::isLetter(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

