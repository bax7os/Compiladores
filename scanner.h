#pragma once
#include "symboltable.h"
#include <fstream>

class Scanner {
private:
  string input; // Storage the input token
  int pos;      // Current position
  int line;
  SymbolTable *st;

public:
  int getPos();
  void setPos(int pos);
  char peek();
  Scanner(string, SymbolTable *);
  int getLine();
  string getInputString();
  Token *nextToken();
  void lexicalError(string);
};
