#pragma once 
#include <iostream>
#include <ctype.h>
#include <string>

using namespace std;



enum Names 
{
    UNDEF = 0,
    LETTER = 1,
    DIGIT = 2,
    ID = 3,
    INTEGER_LITERAL = 4,
    OPERATOR = 5,
    SEPARATOR = 6,
    COMMENT = 7,
    SPACE = 8,
    RESERVED = 9,
    SYSTEM_OUT_PRINTLN = 10, 
    END_OF_FILE
};

enum TypeEnum { T_UNDEF, T_INT, T_BOOL, T_INT_ARRAY, T_CLASS };


class Token 
{
    public: 
        int name;
        int attribute;
        string lexeme;
    
        Token(int name)
        {
            this->name = name;
            attribute = UNDEF;
        }

        Token(int name, string l)
        {
            this->name = name;
            attribute = UNDEF;
            lexeme = l;
        }
        
        Token(int name, int attr)
        {
            this->name = name;
            attribute = attr;
        }
};
