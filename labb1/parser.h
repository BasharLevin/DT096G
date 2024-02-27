//
// Created by Bashar Levin on 2024-01-31.
//

#ifndef LABB1_PARSER_H
#define LABB1_PARSER_H
#include <string>
#include "lexer.h"

using it = std::string::iterator;

struct op{
    virtual bool eval (it fist, it last, it& ptr) = 0;
    void add(op* child){
        if (child)
            children.push_back(child);
    }
    std::vector<op*> children;
};

struct ch_op:op{
    char c;
    ch_op(char c):c(c){}
    bool eval(it first, it last, it& ptr) override{
        if(*first == c || c =='*' || c == '.'){
            return true;
        }
        else{
            return false;
        }
    }
};
struct word_op:op{
    bool eval(it first, it last, it& ptr) override{
        auto result = children[0]->eval(first, last, ptr);
        if (first != last){
            ptr = ++first;
        }
        if (children.size() > 1){
            return result && children[1]->eval(++first, last, ptr);
        }
        return result;
    }
};
struct string_op:op{
    bool eval(it first, it last, it& ptr) override{
        auto result = children[0]->eval(first, last, ptr);
        if(children.size() > 1){
            return result && children[1]->eval(first + 1, last, ptr);
        }
        return result;
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

struct many:op{
    bool printall = 0;
    bool eval (it first, it last, it& ptr) override{
        static int n = 0;
        static std::string word;
        auto result = children[0]->eval(first, last, ptr);
        if (first == last){
            return false;
        }
        if (!result){
            eval(++first, last, ptr);
        }
        while (first != ptr){
            word+=*first;
            first++;
        }
        if(n<1){
            n++;
            while (first != last){
                if(*first == *ptr || printall){
                    word+=*first;
                    first++;
                }
                else{
                    break;
                }
                std::cout<< word<<std::endl;
            }
        }
        return result;
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
};

struct counter:op{
    int n = 0;
    counter(int c ):n(c){}

    bool eval(it first, it last, it& ptr) override{
        static int b = 0;
        auto result = children[0]->eval(first, last,  ptr);
        if (first == last){
            return false;
        }
        if (!result){
            eval(++ first, last, ptr);
        }
        last = ptr + (n+1);
        if (n<1){
            n++;
        }
        while (first != last){
            std::cout<<*first;
            first++;
        }
        return result;
    }
};
struct expr_op:op{
    bool eval(it first, it last, it& ptr) override{
        if (first == last)
            return false;
        auto result = children[0]->eval(first, last, ptr);
        if (result){
            return true;
        }
        return false;
    }
};


    word_op* parse_word(it& first, it last, lexer lexer);
    group_op* parse_group(it& first, it last, lexer lexer);
    expr_op* parse_expr(it& first, it last, lexer lexer);
    match_op* parse_match(it first, it last, lexer lexer);
    ch_op* parse_char(it& first, it last, lexer lexer);




#endif //LABB1_PARSER_H
