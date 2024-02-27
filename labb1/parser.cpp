//
// Created by Bashar Levin on 2024-01-31.
//

#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <vector>

/*
<match> := <exp>
<exp> := <group> | <or> | <many> | <counter>
<group> := (exp)
<word> := <string> | <char>
<char> := a-z | A-Z | <digits> | <anyChar>
<anyChar> := .
<string> := <word>(<char> <word>) | <digits>
<digits> := 0-9 <digits>
<or> := <exp> + <exp>
<many> := <exp>*
<counter> := <exp>{<digits>}
 */
ch_op* char_parse(it first, it last, lexer lexer){
    auto value = lexer.check(first, last);
    if (value == lexer::END){
        return nullptr;
    }
    if (value == lexer::CHAR || value == lexer::DIGIT){
        return new ch_op(*first);
    }
    return nullptr;
}
word_op* parse_word(it& first, it last, lexer lexer){
    auto value = char_parse(first, last, lexer);
    if (value){
        auto* result = new word_op;
        result->add(value);
        ++first;
        result->add(parse_word(first, last, lexer));
        return result;
    }
    return nullptr;

}
many* many_parser(it first, it last, lexer lexer){
    auto word = parse_word(first, last, lexer);
    if (!word)
        return nullptr;
    char token = lexer.check(first, last);
    if (token == lexer::MANY_OP){
        many* value = new many;
        value->add(word);
        return value;

    }
    return nullptr;
}
counter* count_parser(it& first, it last, lexer lexer){
    auto word = parse_word(first, last, lexer);
    if (!word)
        return nullptr;
    char token = lexer.check(first, last);
    if (token == lexer::LBRACKET) {
        ++first;
        token = lexer.check(first, last);
        if (token == lexer::DIGIT) {
            int n = *first - '0';
            counter *ctr = new counter(n);

            token = lexer.check(++first, last);
            if (token == lexer::LBRACKET) {
                ctr->add(word);
                return ctr;
            }
        }
    }
    return nullptr;
}
or_* or_parser(it& first, it last, lexer lexer){
    auto word = parse_word(first, last, lexer);
    if (word) {
        char token = lexer.check(first, last);
        if (token == lexer::EITHER_OP) {
            or_* either = new or_;
            either->add(word);
            first++;
            word_op* word_2 = parse_word(first, last, lexer);

            if (word_2){
                either->add(word_2);
            } else{
                return nullptr;
            }
            return either;
        }
    }
    return nullptr;
}

group_op* parse_group(it& first, it last, lexer lexer){
    char token = lexer.check(first, last);
    bool sp = false;
    while (token == lexer::SPACE){
        ++first;
        token = lexer.check(first, last);
        sp = true;
    }
    if (sp){
        token = lexer.check(++first, last);
    }
    if (token == lexer::LPAREN){
        ++first;
        auto text = parse_expr(first, last, lexer);
        if (!text){
            return nullptr;
        }
        while (*first != ')'){
            if (first == last)break;
            first++;
        }
        token = lexer.check(first, last);
        if (token != lexer::RPAREN){
            return nullptr;
        }
        auto node = new group_op;
        node->add(text);
        return node;
    }
    return nullptr;
}


expr_op* parse_expr(it& first, it last, lexer lexer){

    auto group_op = parse_group(first, last, lexer);
    if (group_op){
        auto expr_node = new expr_op;
        expr_node->add(group_op);
        expr_node->add(parse_expr(first, last, lexer));
        return expr_node;
    }
    auto text_node = parse_word(first,last, lexer);
    if(text_node){
        auto expr_node = new expr_op;
        expr_node->add(text_node);
        expr_node->add(parse_expr(first,last, lexer));
        return expr_node;
    }
    auto many = many_parser(first, last, lexer);
    if (many){
        auto expr = new expr_op;
        expr->add(many);
        return expr;
    }
    auto or_exp = or_parser(first, last, lexer);
    if (or_exp){
        auto expr = new expr_op;
        expr->add(or_exp);
        return expr;
    }
    auto counter_expr = count_parser(first, last, lexer);
    if (counter_expr){
        auto expr = new expr_op;
        expr->add(counter_expr);
        return expr;
    }
    return nullptr;
}

match_op* parse_match(it first, it last, lexer lexer){
    auto expr_node = parse_expr(first, last, lexer);
    if(!expr_node)
        return nullptr;
    auto result = new match_op;
    result->add(expr_node);
    return result;
}



