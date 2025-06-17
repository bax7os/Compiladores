#pragma once 
#include "symboltable.h"
#include <fstream>

class Scanner 
{
    private: 
        string input;//Storage the input token
        int pos;//Current position
        int line;
        SymbolTable* st;
    
    public:
    //Constructor
        Scanner(string, SymbolTable*);

        int getLine();
        // Method to get the next token
        Token* nextToken();        
    
        // Method to manipulate the error message
        void lexicalError(string);

};
