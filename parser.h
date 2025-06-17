#pragma once

#include "scanner.h"
#include "symboltable.h"
#include <string>

class Parser {
public:
    Parser(Scanner* scanner, SymbolTable* st);
    void parse();

private:
    Scanner* scanner;
    Token* currentToken;
    Token* previousToken;
    SymbolTable* st;

    // Controls Methods
    void nextToken();
    void match(int expectedName);
    void match(const string& expectedLexeme);
    void syntaxError(const string& expected);

    // Functions for Analysis of the Grammar
    void program();
    void mainClass();
    void classDeclaration(SymbolTable* scope);
    void varDeclaration(SymbolTable* scope);
    void methodDeclaration(SymbolTable* scope);
    void params(SymbolTable* scope);
    bool isType();
    void type();
    void statement(SymbolTable* scope);
    
    // Expressions Hierarchy (Top: lower level, bottom: higher level)
    void expression();          // (&&)
    void comparisonExpr();      // (<, ==, !=)
    void additiveExpr();        // (+, -)
    void multiplicativeExpr();  // (*, /)
    void unaryExpr();           //  (!)
    void primaryExpr();         // (new, id, literal, (), [], .length, .id())
    
    void expressionsList();
};