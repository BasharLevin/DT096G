//
// Created by Bashar Levin on 2024-01-31.
//

#ifndef LABB1_LEXER_H
#define LABB1_LEXER_H
#include <iostream>
using itr = std::string::iterator;
class lexer {
public:
    enum token{
        LPAREN, RPAREN, LBRACKET, RBRACKET, EITHER_OP,
        MANY_OP, WILDCARD, DIGIT, LETTER, LEVEL_OP, IGNORE_OP,
        UNKOWN [[maybe_unused]], END
    };

    [[nodiscard]] token get_current_token() const;

    void set_current_token(token token);
    void set_current_token(itr& str, itr& last);
    token get_current(itr& str, itr& last);
    token get_next(itr& str, itr& last);

private:
    token lookup(itr str, itr last);
    token current_token;
};


#endif //LABB1_LEXER_H
