//
// Created by Bashar Levin on 2024-01-31.
//

#include <string>
#include "lexer.h"
using it = std::string::iterator;

int lexer::type = 0;

lexer::lexer(itr first, itr last) {
    while(first != last){
        input += *first;
        ++first;
    }
}

char lexer::check(it first, it last) {
    if (first == last) {
        type = END;
        return ' ';
    }

    switch (*first) {
        case '(':
            type = LPAREN;
            return *first++;
        case ')':
            type = RPAREN;
            return *first++;
        case '{':
            type = LBRACKET;
            return *first++;
        case '}':
            type = RBRACKET;
            return *first++;
        case '+':
            type = EITHER_OP;
            return *first++;
        case '*':
            type = MANY_OP;
            return *first++;
        case '0'...'9':
            type = DIGIT;
            return *first++;
        case 'a'...'z':
        case 'A'...'Z':
            type = CHAR;
            return *first++;
        case '"':
            type = STRING;
            return *first++;
        case ' ':
            type = SPACE;
            return *first++;
        default:
            type = CHAR;
            return getNextChar(first, last);
    }
}

char lexer::getNextChar(itr& first, itr last) {
    char result = *first;
    ++first;
    // Consume characters until a non-alphanumeric character is found
    while (first != last && (isLetter(*first) || isDigit(*first))) {
        result += *first;
        ++first;
    }
    return result;
}

bool lexer::isLetter(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool lexer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}
