//
// Created by Bashar Levin on 2024-01-31.
//


#ifndef UNTITELD_LEXER_H
#define UNTITELD_LEXER_H

#include <iostream>
#include <cctype>
using itr = std::string::iterator;
using token = int;

class lexer {
public:
    enum {
        LPAREN , RPAREN, LBRACKET, RBRACKET, EITHER_OP,
        MANY_OP , DIGIT  , CHAR, DOT, SPACE ,  END , UNKOWN, SLASH
    };
    char getCurrentC() const {
        return current_c;
    }
    std::string getOutputGroupDigits() const {
        return digits;
    }
    static int type;
    char check(itr& first, itr& last);
    std::string input;
    lexer(itr first, itr last);

private:
    // Helper functions
    static char getNextChar(itr& first, itr last);
    static bool isLetter(char c);
    std::string digits;
    char current_c;
};

#endif
