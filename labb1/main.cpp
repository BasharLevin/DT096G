//
// Created by Bashar Levin on 2024-01-31.
//

#include <iostream>
#include "parser.h"
#include "lexer.h"
int main() {

    std::string program = "waterloo";
    std::string input = "Waterloooo Io was defeated, you won the war Waterloo promise to"
                        " love you for ever more Waterloo couldn't escape if I wanted"
                        " to Waterloo knowing was my fate is I to be with you Waterloo finally"
                        " facing my Waterloo ";

    lexer lexer(program.begin(),program.end());
    auto tree = parse_match(program.begin(),program.end(),lexer);
    it first = input.begin();
    it last = input.end();
    it ptr  = first;

    if(tree){
        auto match = tree->eval(first,last,ptr);
        if(match){
            std::cout<<" :FOUND A MATCH";
        } else{
            std::cout<<"NO MATCH FOUND";
        }
    } else std::cout<<"Wrong input";



    return 0;
}