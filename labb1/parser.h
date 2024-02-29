//
// Created by Bashar Levin on 2024-01-31.
//

#ifndef UNTITlED_PARSER_H
#define UNTITlED_PARSER_H
#include <string>
#include "lexer.h"

using it = std::string::iterator;

struct MatchResult {
    bool success = false;
    it start;
    it end;
};
struct op{
    virtual MatchResult eval(it first, it last, it& ptr) = 0;
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
    MatchResult eval(it first, it last, it& ptr) override{
        if (!children.empty() && children[0] != nullptr) {
            MatchResult result = children[0]->eval(first, last, ptr);
            return result;
        }
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
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        if (first != last && (*first == c || c == '.')) {
            result.success = true;
            result.start = first;
            result.end = std::next(first);
            ptr = result.end; // Move the pointer forward
        }
        return result;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << " ";
        std::cout << "ch_op: '" << c << "'\n";
        op::print_tree(depth); // Call base class print to handle children if any
    }
};
struct wildCard: ch_op {
    wildCard(): ch_op('\0'){} // Initializer list calling base class constructor

    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        if (first != last) { // Ensure there's at least one character to match
            result.success = true;
            result.start = first;
            result.end = std::next(first); // Match one character
            ptr = result.end; // Update the pointer to the new position
        }
        return result;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << " ";
        std::cout << "wildcard: '" << c << "'\n";
        op::print_tree(depth); // Call base class print to handle children if any
    }
};

struct word_op:op{
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        if (children.empty()) {
            result.success = false;
            return result; // Return early if there are no children
        }

        // Evaluate the first child
        auto childResult = children[0]->eval(first, last, ptr);
        if (!childResult.success) {
            return childResult; // If the first child doesn't match, fail early
        }

        // Initialize the match boundaries with the result of the first child
        result.start = childResult.start;
        result.end = childResult.end;
        result.success = true; // Assuming success unless a child fails

        // Sequentially evaluate the rest of the children
        for (size_t i = 1; i < children.size(); ++i) {
            first = result.end; // Start from where the last match ended
            childResult = children[i]->eval(first, last, ptr);

            if (!childResult.success) {
                result.success = false; // Fail if any child fails
                break; // Exit the loop on failure
            }

            // Extend the match boundary to include the current child's match
            result.end = childResult.end;
        }

        // Update ptr to the end of the match if successful
        if (result.success) {
            ptr = result.end;
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

    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        // Check if the group index is out of bounds
        if (group_index <= 0 || group_index > children.size()) {
            result.success = false; // Mark as failed match
            return result;
        }
        op* target_group = children[group_index - 1];

        // Execute the target group's eval and return its result
        result = target_group->eval(first, last, ptr);

        // The success, start, and end of the match are determined by the target group's evaluation
        return result;
    }
    void print_tree(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "output_group_op: group_index = "  << "\n";
        for (const auto& child : children) {
            if (child) child->print_tree(depth + 1);
        }
    }
};
struct group_op: op {
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        // Check if we've reached the end of the input
        if (first == last) {
            result.success = false; // Indicate failure if there's no input left
            return result;
        }
        result = children[0]->eval(first, last, ptr);
        if (result.success) {
            ptr = result.end;
        }
        return result;
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
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        if (first == last) {
            result.success = false;
            return result;
        }

        result = children[0]->eval(first, last, ptr);
        if (!result.success && std::next(first) != last) {
            return eval(std::next(first), last, ptr);
        }
        return result;
    }
};
struct many: op{
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        result.success = false; // Assume failure initially
        auto start = first;

        while (first != last) {
            auto childResult = children[0]->eval(first, last, ptr);
            if (!childResult.success) {
                break; // Stop if the child no longer matches
            }
            // Ensure at least one match occurred for success
            result.success = true;
            first = ptr; // Update 'first' to the new position
        }

        if (result.success) {
            result.start = start;
            result.end = first; // Set the end of the match
        }

        // Evaluate the second child if it exists and the first child matched at least once
        if (result.success && children.size() > 1) {
            auto secondChildResult = children[1]->eval(start, last, ptr);
            if (!secondChildResult.success) {
                result.success = false; // Mark as failure if the second child fails
            }
        }

        return result;
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
    MatchResult eval(it first, it last, it& ptr) override {
        auto result = children[0]->eval(first, last, ptr);
        if (result.success) {
            return result;
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
    counter(int c) : N(c) {}

    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        if (std::distance(first, last) < N) {
            result.success = false;
            return result; // Return early if there aren't enough characters left to match N times
        }

        int count = 0;
        it start = first; // Remember the start position for potential rollback
        it matchEnd = first; // To track the end of the last successful match

        // Try to match exactly N occurrences of the specified character/pattern
        while (first != last && count < N) {
            MatchResult childResult = children[0]->eval(first, last, ptr);
            if (childResult.success) {
                // Successfully matched one occurrence, move to the next character
                ++count;
                matchEnd = childResult.end; // Update matchEnd to the end of this successful match
                first = matchEnd; // Prepare first for the next match
            } else {
                // Failed to match the required character/pattern
                break;
            }
        }

        if (count == N) {
            // Matched exactly N occurrences
            result.success = true;
            result.start = start;
            result.end = matchEnd; // End of the last successful match
            ptr = matchEnd; // Update ptr to reflect the new position
        } else {
            // Reset first to its original position if the match was not successful
            first = start;
            result.success = false;
        }

        return result;
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
    MatchResult eval(it first, it last, it& ptr) override {
        MatchResult result;
        // Check if we've reached the end of the input
        if (first == last) {
            result.success = false; // No match if there's no input left
            return result;
        }
        // Evaluate the first (and presumably only) child
        result = children[0]->eval(first, last, ptr);
        // The success of this operation is directly dependent on the child's success
        return result;
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
