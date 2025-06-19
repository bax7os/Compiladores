#include "stentry.h"

STEntry::STEntry(Token *token, bool reserved) {
  this->token = token;
  this->reserved = reserved;
}

STEntry::STEntry(Token *token) {
  this->token = token;
  this->reserved = false;
}

STEntry::STEntry(Token *token, string type) {
  this->token = token;
  this->type = type;
  this->reserved = false;
}

STEntry::STEntry(Token *token, string returnType, vector<STEntry *> params) {
  this->token = token;
  this->isMethod = true;
  this->returnType = returnType;
  this->params = params;
  this->reserved = false;
  this->type = "method";
}