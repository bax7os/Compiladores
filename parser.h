#pragma once

#include "scanner.h"
#include "symboltable.h"
#include <string>
#include <vector>

class Parser {
public:
  Parser(Scanner *scanner, SymbolTable *st);
  void parse();
  bool hasSemanticErrors() const { return !semanticErrors.empty(); }
  void printSemanticErrors() const;

private:
  Scanner *scanner;
  Token *currentToken;
  Token *previousToken;
  SymbolTable *st;
  vector<string> semanticErrors;

  // Helper methods
  void addSemanticError(const string &msg);
  bool checkVariableExists(const string &name, SymbolTable *currentScope);
  bool checkTypesCompatible(Token *type1, Token *type2);
  bool isBooleanType(Token *type);
  bool isIntegerType(Token *type);

  // Controls Methods
  void nextToken();
  void match(int expectedName);
  void match(const string &expectedLexeme);
  void syntaxError(const string &expected);

  // Functions for Analysis of the Grammar
  void program();
  void mainClass();
  void classDeclaration(SymbolTable *scope);
  void varDeclaration(SymbolTable *scope);
  void methodDeclaration(SymbolTable *scope, const string &currentClassName);
  vector<STEntry *> params(SymbolTable *scope);
  bool isType();
  string type();
  void statement(SymbolTable *scope, const string &currentClassName);

  // Expressions Hierarchy
  Token *expression(SymbolTable *scope, const string &currentClassName);
  Token *comparisonExpr(SymbolTable *scope, const string &currentClassName);
  Token *additiveExpr(SymbolTable *scope, const string &currentClassName);
  Token *multiplicativeExpr(SymbolTable *scope, const string &currentClassName);
  Token *unaryExpr(SymbolTable *scope, const string &currentClassName);
  Token *primaryExpr(SymbolTable *scope, const string &currentClassName);

  vector<Token *> expressionsList(SymbolTable *scope,
                                  const string &currentClassName);
};