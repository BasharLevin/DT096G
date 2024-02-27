//
// Created by Bashar Levin on 2024-01-31.
//

#ifndef UNTITlED_PARSER_H
#define UNTITlED_PARSER_H
#include <string>
#include "lexer.h"

using it = std::string::iterator;


struct op{
    virtual bool eval(it first, it last, it& ptr) = 0;
    void add(op* child){
        if (child)
            children.push_back(child);
    }
    void pop(op* child){
        if (child)
            children.pop_back();
    }
    std::vector<op*> children;
    virtual void print_tree(int depth = 0) const {
        for (int i = 0; i < depth; ++i) std::cout << " ";
        std::cout << "op\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};

struct subexpr:op{
    bool eval(it first, it last, it& ptr) override{
        if (!children.empty() && children[0] != nullptr) {
            bool result = children[0]->eval(first, last, ptr);
            return result;
        }
        return false;

    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  "; // Indentation for readability
        std::cout << "subexpr\n";
        // Call print_tree on each child, if any
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};

struct ch_op:op{
    char c;
    ch_op(char c):c(c){}
    bool eval(it first, it last, it& ptr) override{
        if(*first == c || c == '.'){
            return true;
        }
        else{
            return false;
        }
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << " ";
        std::cout << "ch_op: '" << c << "'\n";
        op::print_tree(depth); // Call base class print to handle children if any
    }
};
struct wildCard: ch_op{
    wildCard(): ch_op('\0'){}
    bool eval(it first, it last, it& ptr) override{
        return true;
    }
};
struct word_op:op{
    bool eval(it first, it last, it& ptr) override{
        auto result = children[0]->eval(first, last, ptr);
        if (first != last){
            ptr = first;
        }
        if (children.size() > 1){
            return result && children[1]->eval(++first, last, ptr);
        }
        return result;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  "; // Indentation for readability
        std::cout << "word_op\n";
        // Call print_tree on each child, if any
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }

};
struct output_group_op : op {
    int group_index;

    output_group_op(int idx) : group_index(idx) {}

    bool eval(it first, it last, it& ptr) override {
        if (group_index <= 0 || group_index > children.size()) return false;
        op* target_group = children[group_index - 1];
        return target_group->eval(first, last, ptr);
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "output_group_op: group_index = "  << "\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};
struct group_op:op{
    bool eval(it first, it last, it& ptr) override{
        if(first == last)
            return false;
        auto result = children[0]->eval(first, last, ptr);
        if (result)return true;
        return false;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "group_op:\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};
struct match_op:op{
    bool eval(it first, it last, it& ptr) override{
        if (first == last)
            return false;
        auto result = children[0]->eval(first,last, ptr);
        if (!result){
            return eval(first + 1, last, ptr);
        }
        return true;
    }
};
struct many: op{
    bool eval(it first, it last, it& ptr) override {
        bool matchedAtLeastOnce = false;
        auto start = first;

        while (first != last && children[0]->eval(first, last, ptr)) {
            matchedAtLeastOnce = true;
            if (first == ptr) { // Prevent infinite loop by ensuring progress
                ++first;
            } else {
                ptr = first; // Update ptr to the new position after a successful match
            }
        }

        // If there's a second child, try to evaluate it after matching the first child.
        if (matchedAtLeastOnce && children.size() > 1) {
            return children[1]->eval(start, last, ptr);
        }

        return matchedAtLeastOnce;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  "; // Indentation for readability
        std::cout << "many\n";
        // Call print_tree on each child, if any
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }

};
struct or_:op{
    bool eval(it first, it last, it& ptr) override{
        auto result = children[0]->eval(first, last, ptr);
        if (result){
            return true;
        }
        return children[1]->eval(first, last, ptr);
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  "; // Indentation for readability
        std::cout << "or\n";
        // Call print_tree on each child, if any
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};

struct counter:op{
    int N = 0;
    counter(int c):N(c){}
    bool eval(it first, it last, it& ptr) override {
        if (std::distance(first, last) < N) {
            return false;
        }
        int count = 0;
        it start = first; // Remember the start position.
        // Try to match exactly N occurrences of the specified character/pattern.
        while (first != last && count < N) {
            if (children[0]->eval(first, last, ptr)) {
                // Successfully matched one occurrence, move to the next character.
                ++count;
                ++first;
                ptr = first; // Update ptr to reflect the new position.
            } else {
                // Failed to match the required character/pattern.
                break;
            }
        }

        if (count == N) {
            // Matched exactly N occurrences.
            return true;
        } else {
            // Reset first to its original position if the match was not successful.
            first = start;
            return false;
        }
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "counter\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};
struct expr_op : op {
    bool eval(it first, it last, it& ptr) override{
        if(first == last){return false;}
        auto result = children[0]->eval(first, last, ptr);
        if(result){
            return true;
        }
        return false;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "expression\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};



word_op* parse_word(it& first, it last, lexer lexer);
expr_op* parse_expr(it& first, it last, lexer lexer);
match_op* parse_match(it first, it last, lexer lexer);
or_* parse_or(it first, it last, lexer lexer);
subexpr* parse_subexpression(it& first, it last, lexer lexer);
wildCard* wild_card(it first, it last, lexer lexer);
ch_op* char_parser(it first, it last, lexer lexer);
counter* count(it& first, it last,lexer lexer);
many* parse_many(it& first, it last, lexer& lexer);
output_group_op* parse_output_group(it& first, it last, lexer& lexer);




#endif //LABB1_PARSER_H
