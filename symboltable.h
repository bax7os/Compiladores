#pragma once
#include "stentry.h"
#include <map>
#include <string>
class STEntry;

// Class that represents a symbol table. The class uses the map class to store
// the symbols. Map implements a tree that associates a key with a value: in our
// case the key is a string and the value is a pointer to an object of the class
// STEntry.
class SymbolTable {
private:
  SymbolTable *parent;            // References to the parent symbol table.
  map<string, STEntry *> symbols; // Storage of symbols in the current scope.

public:
  SymbolTable();
  SymbolTable(SymbolTable *);

  bool add(STEntry *);
  bool remove(string);
  void clear();
  bool isEmpty();
  STEntry *get(string);
  SymbolTable *getParent();
};
