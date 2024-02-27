#include <iostream>

#include <iostream>
#include "parser.h"
#include "lexer.h"
int main() {

    std::string program = "o*";
    std::string input = "Waterloooo Io was defeated, you won the war Waterloo promise to"
                        " love you for ever more Waterloo couldn't escape if I wanted"
                        " to Waterloo knowing was my fate is I to be with you Waterloo finally"
                        " facing my Waterloo ";

    lexer lexer(program.begin(), program.end());
    auto tree = parse_match(program.begin(), program.end(), lexer);
    it first = input.begin();
    it last = input.end();
    it ptr = first;

    if (tree) {
        auto match = tree->eval(first, last, ptr);
        if (match) {
            tree->print_tree();
            std::cout << " :FOUND A MATCH";
        } else {
            std::cout << "NO MATCH FOUND";
        }
    } else std::cout << "Wrong input" << std::endl;
    return 0;
}
