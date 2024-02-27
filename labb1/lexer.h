//
// Created by Bashar Levin on 2024-01-31.
//


#ifndef LABB1_LEXER_H
#define LABB1_LEXER_H

#include <iostream>
#include <cctype>
using itr = std::string::iterator;

class lexer {
public:
    enum {
        LPAREN, RPAREN, LBRACKET, RBRACKET, EITHER_OP,
        MANY_OP, DIGIT, CHAR, STRING, SPACE,  END
    };
    static int type;
    static char check(itr first, itr last);
    std::string input;
    lexer(itr first, itr last);

private:
    // Helper functions
    static char getNextChar(itr& first, itr last);
    static bool isLetter(char c);
    static bool isDigit(char c);
};

#endif //LABB1_LEXER_H
