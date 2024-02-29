//
// Created by Bashar Levin on 2024-01-31.
//

#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <vector>

/*
<match> ::= <exp>
<exp> ::= <group> | <or> | <sub> | <word>
<sub> ::= <many> | <counter>
<group> ::= '(' <exp> ')'
<word> ::= <char> | <char> <word> | <sub>
<char> ::= 'a'-'z' | 'A'-'Z' | <digits> | <wildcard>
<digits> ::= '0'-'9' { <digits> }
<or> ::= <exp> '+' <exp>
<many> ::= <exp> '*'
<counter> ::= <exp> '{' <digits> '}'
<ignore> ::= <exp> '\I'
<output_group> ::= <gropu>\O{ditgits}
<wildcard> ::= '.'
 */


many* parse_many(it& first, it last, lexer& lexer) {
    auto restore = first;
    if (first == last) return nullptr;

    auto word = char_parser(first, last, lexer);
    if (!word) {
        return nullptr;
    }
    auto token = lexer.check(++first, last);
    if (token != lexer::MANY_OP) {
        first = restore;
        return nullptr;

    }

    many* value = new many;
    value->add(word);

    ++first;
    if (first != last) {
        auto nextExpr = parse_expr(first, last, lexer);
        if (nextExpr) {
            value->add(nextExpr);
        }
    }

    return value;
}

wildCard* wild_card(it first, it last, lexer lexer){
    char token = lexer.check(first, last);
    if (token == lexer::DOT){
        return new wildCard;
    }
    return nullptr;
}

or_* parse_or(it first, it last, lexer lexer){
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
ch_op* char_parser(it first, it last, lexer lexer){
    char token = lexer.check(first, last);
    auto result = wild_card(first, last, lexer);
    if (token == lexer::END)
        return nullptr;
    if (result) {
        auto* wc = new wildCard;
        wc->add(result);
        return wc;
    }
    if (token == lexer::CHAR || token == lexer::DIGIT || token == lexer::SPACE){
        return new ch_op(*first);
    }
    return nullptr;
}
subexpr* parse_subexpression(it& first, it last, lexer lexer) {
    many* manyResult = parse_many(first, last, lexer);
    if (manyResult) {
        subexpr* value = new subexpr();
        value->add(manyResult);
        return value;
    }
    counter* counterResult = count(first, last, lexer);
    if (counterResult) {
        subexpr* value = new subexpr();
        value->add(counterResult);
        return value;
    }
    return nullptr;
}

word_op* parse_word(it& first, it last, lexer lexer){
    auto sub = parse_subexpression(first, last, lexer);
    if (sub){
        word_op* result = new word_op;
        result->add(sub);
        return result;
    }
    auto ch = char_parser(first, last, lexer);
    if (ch){
        auto* value = new word_op;
        value->add(ch);
        first++;
        value->add(parse_word(first, last, lexer));
        return value;
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
        first++;
        return node;

    }
    return nullptr;
}
output_group_op* parse_output_group(it& first, it last, lexer& lexer) {
    auto start = first; // Save the starting position in case we need to revert
    auto token = lexer.check(first, last);

    if (token == lexer::SLASH) { // Assuming SLASH is defined and represents '/'
        ++first; // Advance past the slash
        token = lexer.check(first, last);

        if (first != last && *first == 'O') { // Check if 'O' follows the slash directly
            ++first; // Advance past 'O'
            token = lexer.check(first, last);

            if (token == lexer::LBRACKET) { // Assuming LBRACKET represents '{'
                ++first; // Advance past the opening bracket
                std::string number;

                while (first != last && isdigit(*first)) { // Assuming the digits are to be directly read
                    number += *first;
                    ++first; // Collect all digits
                }

                if (lexer.check(first, last) != lexer::RBRACKET) { // Assuming RBRACKET represents '}'
                    throw std::runtime_error("Expected }");
                }
                ++first; // Advance past the closing bracket

                int group_index = std::stoi(number); // Convert collected number to integer
                return new output_group_op(group_index); // Create and return a new output_group_op
            }
        }
    }

    first = start;
    return nullptr;
}


counter* count(it& first, it last, lexer lexer) {
    auto restore = first;
    auto word = char_parser(first, last, lexer);
    if (!word) {
        return nullptr;
    }
    first++;
    auto value = lexer.check(first, last);
    if (value == lexer::LBRACKET) {
        ++first;

        std::string numberStr;
        while ((value = lexer.check(first, last)) == lexer::DIGIT) {
            numberStr += *first;
            ++first; // Collect all digits
        }

        if (!numberStr.empty() && lexer.check(first, last) == lexer::RBRACKET) {
            ++first; // Move past the
            int count = std::stoi(numberStr);

            counter* counterNode = new counter(count);
            counterNode->add(word);

            return counterNode;
        }
    }

    first = restore;
    return nullptr;
}

expr_op* parse_expr(it& first, it last, lexer lexer){
    auto group_node = parse_group(first, last, lexer);
    if (group_node){
        auto group_expr = new expr_op;
        group_expr->add(parse_expr(first, last, lexer));
        auto grout_output = parse_output_group(first, last, lexer);
        if (grout_output){
            auto groupop_expr = new expr_op;
            groupop_expr->add(group_node);
            groupop_expr->add(grout_output);
            return groupop_expr;
        }
        else{}
        group_expr->add(group_node);
        return group_expr;
    }

    auto or_node = parse_or(first, last, lexer);
    if (or_node){
        auto or_expr = new expr_op;
        or_expr->add(or_node);
        return or_expr;
    }
    auto text_node = parse_word(first, last, lexer );
    if (text_node) {
        auto epxr = new expr_op;
        epxr->add(text_node);
        epxr->add(parse_expr(first, last, lexer));
        return epxr;
    }
    return nullptr;

}

match_op* parse_match(it first, it last, lexer lexer) {
    auto expr_node = parse_expr(first, last, lexer);
    if (expr_node) {
        auto result = new match_op;
        result->add(expr_node);
        return result;
    }
    return nullptr;
}

