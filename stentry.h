#pragma once
#include "token.h"
#include <string>
#include <vector>

class SymbolTable;
class STEntry {
public:
  Token *token;
  string type;
  bool reserved;

  bool isMethod = false;
  string returnType;
  vector<STEntry *> params;

  SymbolTable *classScope = nullptr;
  STEntry(Token *token, string type);

  STEntry(Token *token, string returnType, vector<STEntry *> params);

  STEntry(Token *, bool);
  STEntry(Token *);
};
