#include "parser.h"
#include <iostream>

Parser::Parser(Scanner *scanner, SymbolTable *st) {
  this->scanner = scanner;
  this->st = st;
  this->currentToken = nullptr;
  this->previousToken = nullptr;
  nextToken();
}

// Goes to the next token
void Parser::nextToken() {
  previousToken = currentToken;
  currentToken = scanner->nextToken();
}

// Throws a syntax error and ends the execution
void Parser::syntaxError(const string &expected) {
  cout << "ERRO DE SINTAXE na linha " << scanner->getLine() << "." << endl;
  cout << "  >> Esperado: " << expected << endl;
  cout << "  >> Encontrado: '" << currentToken->lexeme << "'" << endl;
  exit(1);
}

// Compares the current token with the expected name
void Parser::match(int expectedName) {
  if (currentToken->name == expectedName) {
    nextToken();
  } else {
    string expectedStr;
    switch (expectedName) {
    case ID:
      expectedStr = "um identificador";
      break;
    case INTEGER_LITERAL:
      expectedStr = "um numero inteiro";
      break;
    case SEPARATOR:
      expectedStr = "um separador (ex: '{', ';', ...)";
      break;
    case SYSTEM_OUT_PRINTLN:
      expectedStr = "'System.out.println'";
      break;
    case END_OF_FILE:
      expectedStr = "fim do arquivo";
      break;
    case OPERATOR:
      expectedStr = "um operador (ex: +, -, *, <, &&)";
      break;
    default:
      expectedStr = "token desconhecido";
    }
    syntaxError(expectedStr);
  }
}
// Adds a semantic error
void Parser::addSemanticError(const string &msg) {
  semanticErrors.push_back("ERRO SEMANTICO na linha " +
                           to_string(scanner->getLine()) + ": " + msg);
}

// Checks if a variable exists
bool Parser::checkVariableExists(const string &name,
                                 SymbolTable *currentScope) {
  STEntry *entry = currentScope->get(name);
  if (entry == nullptr) {
    addSemanticError("Variavel '" + name + "' nao declarada neste escopo");
    return false;
  }
  return true;
}
// Checks if two types are compatible
bool Parser::checkTypesCompatible(Token *type1, Token *type2) {

  if (type1->lexeme == type2->lexeme)
    return true;
  addSemanticError("Tipos incompatíveis: " + type1->lexeme + " e " +
                   type2->lexeme);
  return false;
}
// Checks if a type is boolean
bool Parser::isBooleanType(Token *type) { return type->lexeme == "boolean"; }
// Checks if a type is integer
bool Parser::isIntegerType(Token *type) {
  return type->lexeme == "int" || type->lexeme == "int[]";
}

// Compares the current token with the expected lexeme (for reserved words and
// operators)
void Parser::match(const string &expectedLexeme) {
  if (currentToken->lexeme == expectedLexeme) {
    nextToken();
  } else {
    syntaxError("'" + expectedLexeme + "'");
  }
}
// Starts the analysis
void Parser::parse() {

  Scanner preScannerPass1(scanner->getInputString(), this->st);
  Token *preToken1 = preScannerPass1.nextToken();
  while (preToken1->name != END_OF_FILE) {
    if (preToken1->lexeme == "class") {
      preToken1 = preScannerPass1.nextToken();
      if (preToken1->name == ID) {
        STEntry *classEntry =
            new STEntry(new Token(ID, preToken1->lexeme), true);
        st->add(classEntry);
        classEntry->classScope = new SymbolTable(st);
      }
    }
    preToken1 = preScannerPass1.nextToken();
  }

  Scanner preScannerPass2(scanner->getInputString(), this->st);
  Token *preToken2 = preScannerPass2.nextToken();
  while (preToken2->name != END_OF_FILE) {
    if (preToken2->lexeme == "class") {
      preToken2 = preScannerPass2.nextToken();
      STEntry *classEntry = st->get(preToken2->lexeme);
      SymbolTable *classScope = classEntry->classScope;

      preToken2 = preScannerPass2.nextToken();
      if (preToken2->lexeme != "{")
        continue;

      while (preToken2->lexeme != "}") {
        if (preToken2->lexeme == "public") {

          preToken2 = preScannerPass2.nextToken();
          string returnTypeStr = preToken2->lexeme;
          if (returnTypeStr == "int" && preScannerPass2.peek() == '[') {
            returnTypeStr += "[]";
            preScannerPass2.nextToken();
            preScannerPass2.nextToken();
          }

          preToken2 = preScannerPass2.nextToken();
          if (preToken2->name != ID)
            continue;
          Token *methodNameToken = new Token(ID, preToken2->lexeme);

          preToken2 = preScannerPass2.nextToken();

          vector<STEntry *> paramList;
          if (preToken2->lexeme == "(") {
            preToken2 = preScannerPass2.nextToken();
            while (preToken2->lexeme != ")") {
              string paramTypeStr = preToken2->lexeme;
              if (paramTypeStr == "int" && preScannerPass2.peek() == '[') {
                paramTypeStr += "[]";
              }

              paramList.push_back(new STEntry(nullptr, paramTypeStr));

              while (preToken2->lexeme != "," && preToken2->lexeme != ")") {
                preToken2 = preScannerPass2.nextToken();
              }
              if (preToken2->lexeme == ",")
                preToken2 = preScannerPass2.nextToken();
            }
          }

          STEntry *methodEntry =
              new STEntry(methodNameToken, returnTypeStr, paramList);
          classScope->add(methodEntry);

          preToken2 = preScannerPass2.nextToken();
          if (preToken2->lexeme == "{") {
            int braceCount = 1;
            while (braceCount > 0) {
              preToken2 = preScannerPass2.nextToken();
              if (preToken2->lexeme == "{")
                braceCount++;
              if (preToken2->lexeme == "}")
                braceCount--;
              if (preToken2->name == END_OF_FILE)
                break;
            }
          }
        }
        preToken2 = preScannerPass2.nextToken();
        if (preToken2->name == END_OF_FILE)
          break;
      }
    } else {
      preToken2 = preScannerPass2.nextToken();
    }
  }

  program();
  if (hasSemanticErrors()) {
    printSemanticErrors();
  } else {
    cout << "Syntax analyzer and semantic analyzer completed!" << endl;
  }
}

// Program -> MainClass (ClassDeclaration)* EOF
void Parser::program() {
  mainClass();
  while (currentToken->lexeme == "class") {
    classDeclaration(this->st);
  }
  match(END_OF_FILE);
}

// MainClass -> class ID { public static void main (String [] ID){ Statement } }
void Parser::mainClass() {
  match("class");
  Token *mainClassNameToken = currentToken;
  match(ID);

  match("{");
  match("public");
  match("static");
  match("void");
  match("main");
  match("(");
  match("String");
  match("[");
  match("]");
  match(ID);
  match(")");
  match("{");
  SymbolTable *mainScope = new SymbolTable(this->st);

  statement(mainScope, mainClassNameToken->lexeme);

  delete mainScope;
  match("}");
  match("}");
}

// ClassDeclaration -> class ID (extends ID)? { (VarDeclaration)*
// (MethodDeclaration)* }
void Parser::classDeclaration(SymbolTable *scope) {
  match("class");
  string className = currentToken->lexeme;
  match(ID);

  STEntry *classEntry = st->get(className);
  SymbolTable *classScope = classEntry->classScope;

  if (currentToken->lexeme == "extends") {
    nextToken();
    match(ID);
  }

  match("{");
  while (isType()) {
    varDeclaration(classScope);
  }
  while (currentToken->lexeme == "public") {
    methodDeclaration(classScope, className);
  }

  match("}");
}
// VarDeclaration -> Type ID;
void Parser::varDeclaration(SymbolTable *scope) {
  string varType = type();
  scope->add(new STEntry(new Token(ID, currentToken->lexeme), varType));

  match(ID);
  match(";");
}
// MethodDeclaration -> public Type ID ( (Params)? ) { (VarDeclaration)*
// (Statement)* return Expression; }
void Parser::methodDeclaration(SymbolTable *scope,
                               const string &currentClassName) {
  match("public");
  type();
  match(ID);
  match("(");

  SymbolTable *methodScope = new SymbolTable(scope);

  if (currentToken->lexeme != ")") {
    params(methodScope);
  }
  match(")");
  match("{");

  while (isType()) {
    varDeclaration(methodScope);
  }

  while (currentToken->lexeme != "return" && currentToken->lexeme != "}") {
    statement(methodScope, currentClassName);
  }

  match("return");
  expression(methodScope, currentClassName);
  match(";");
  match("}");
  delete methodScope;
}
// Params -> Type ID (, Type ID)*
vector<STEntry *> Parser::params(SymbolTable *scope) {
  vector<STEntry *> paramList;

  string paramTypeStr = type();
  Token *paramName = currentToken;
  match(ID);

  STEntry *paramEntry = new STEntry(paramName, paramTypeStr);
  scope->add(paramEntry);
  paramList.push_back(paramEntry);

  while (currentToken->lexeme == ",") {
    nextToken();
    paramTypeStr = type();
    paramName = currentToken;
    match(ID);
    paramEntry = new STEntry(paramName, paramTypeStr);
    scope->add(paramEntry);
    paramList.push_back(paramEntry);
  }
  return paramList;
}

// Helper that verifies if the current token can be the start of an type
bool Parser::isType() {
  if (currentToken->lexeme == "int" || currentToken->lexeme == "boolean") {
    return true;
  }

  if (currentToken->name == ID) {
    STEntry *entry = st->get(currentToken->lexeme);
    if (entry != nullptr && entry->reserved) {
      return true;
    }
  }
  return false;
}

// Type -> int []? | boolean | ID
string Parser::type() {
  if (!isType()) {
    syntaxError("um tipo (int, boolean, ou ID de classe)");
  }

  string typeStr = currentToken->lexeme;
  nextToken();

  if (typeStr == "int" && currentToken->lexeme == "[") {
    nextToken();
    match("]");
    return "int[]";
  }

  return typeStr;
}

// Statement -> { (Statement)* } | if ... | while ... | System.out.println ... |
// ID ...
void Parser::statement(SymbolTable *scope, const string &currentClassName) {
  if (currentToken->lexeme == "{") {
    nextToken();
    while (currentToken->lexeme != "}") {
      statement(scope, currentClassName);
    }
    match("}");
  } else if (currentToken->lexeme == "if") {
    nextToken();
    match("(");
    Token *exprType = expression(scope, currentClassName);
    if (!isBooleanType(exprType)) {
      addSemanticError("Condição do if deve ser booleana");
    }
    match(")");
    statement(scope, currentClassName);
    match("else");
    statement(scope, currentClassName);
  } else if (currentToken->lexeme == "while") {
    nextToken();
    match("(");
    expression(scope, currentClassName);
    match(")");
    statement(scope, currentClassName);
  } else if (currentToken->name == SYSTEM_OUT_PRINTLN) {
    nextToken();
    match("(");
    expression(scope, currentClassName);
    match(")");
    match(";");

  } else if (currentToken->name == ID) {
    Token *varToken = currentToken;
    match(ID);
    checkVariableExists(varToken->lexeme, scope);

    if (currentToken->lexeme == "=") {
      match("=");

      STEntry *lhsEntry = scope->get(varToken->lexeme);
      Token *rhsType = expression(scope, currentClassName);

      if (lhsEntry != nullptr && lhsEntry->type != rhsType->lexeme) {
        addSemanticError("Tipos incompativeis: tentando atribuir o tipo '" +
                         rhsType->lexeme + "' a uma variavel do tipo '" +
                         lhsEntry->type + "'");
      }

      match(";");

    } else if (currentToken->lexeme == "[") {
      match("[");
      Token *indexType = expression(scope, currentClassName);
      if (!isIntegerType(indexType)) {
        addSemanticError("Índice de array deve ser inteiro");
      }
      match("]");
      match("=");

      STEntry *lhsEntry = scope->get(varToken->lexeme);
      Token *rhsType = expression(scope, currentClassName);

      if (lhsEntry != nullptr) {
        if (lhsEntry->type.find("[]") == string::npos) {
          addSemanticError("Tentativa de acesso de array ('[]') em uma "
                           "variavel nao-array do tipo '" +
                           lhsEntry->type + "'");
        } else {
          string lhsBaseType =
              lhsEntry->type.substr(0, lhsEntry->type.find("[]"));
          if (lhsBaseType != rhsType->lexeme) {
            addSemanticError("Tipos incompativeis: tentando atribuir o tipo '" +
                             rhsType->lexeme +
                             "' a um elemento de um array de base '" +
                             lhsBaseType + "'");
          }
        }
      }

      match(";");
    } else {
      syntaxError(
          "um '=' ou '[' apos o identificador em um comando de atribuicao");
    }
  } else {
    syntaxError("um comando ('{', 'if', 'while', 'System.out.println' ou um ID "
                "de variavel)");
  }
}

// expression -> comparisonExpr ( && comparisonExpr )*
Token *Parser::expression(SymbolTable *scope, const string &currentClassName) {
  Token *leftType = comparisonExpr(scope, currentClassName);
  while (currentToken->lexeme == "&&") {
    if (!isBooleanType(leftType)) {
      addSemanticError("Operador && requer operandos booleanos");
    }
    nextToken();
    Token *rightType = comparisonExpr(scope, currentClassName);
    if (!isBooleanType(rightType)) {
      addSemanticError("Operador && requer operandos booleanos");
    }
    leftType = new Token(RESERVED, "boolean");
  }
  return leftType;
}

// comparisonExpr -> additiveExpr ( ( < | > | == | != ) additiveExpr )?
Token *Parser::comparisonExpr(SymbolTable *scope,
                              const string &currentClassName) {
  Token *leftType = additiveExpr(scope, currentClassName);
  if (currentToken->lexeme == "<" || currentToken->lexeme == ">" ||
      currentToken->lexeme == "==" || currentToken->lexeme == "!=") {

    if (!isIntegerType(leftType)) {
      addSemanticError("Operador de comparacao requer operandos inteiros");
    }

    nextToken();
    Token *rightType = additiveExpr(scope, currentClassName);

    if (!isIntegerType(rightType)) {
      addSemanticError("Operador de comparacao requer operandos inteiros");
    }

    leftType = new Token(RESERVED, "boolean");
  }
  return leftType;
}

// additiveExpr -> multiplicativeExpr ( ( + | - ) multiplicativeExpr )*
Token *Parser::additiveExpr(SymbolTable *scope,
                            const string &currentClassName) {
  Token *leftType = multiplicativeExpr(scope, currentClassName);
  while (currentToken->lexeme == "+" || currentToken->lexeme == "-") {
    if (!isIntegerType(leftType)) {
      addSemanticError("Operador aritmetico requer operandos inteiros");
    }

    nextToken();
    Token *rightType = multiplicativeExpr(scope, currentClassName);

    if (!isIntegerType(rightType)) {
      addSemanticError("Operador aritmetico requer operandos inteiros");
    }

    leftType = new Token(RESERVED, "int");
  }
  return leftType;
}

// multiplicativeExpr -> unaryExpr ( ( * | / ) unaryExpr )*
Token *Parser::multiplicativeExpr(SymbolTable *scope,
                                  const string &currentClassName) {
  Token *leftType = unaryExpr(scope, currentClassName);
  while (currentToken->lexeme == "*" || currentToken->lexeme == "/") {
    if (!isIntegerType(leftType)) {
      addSemanticError("Operador multiplicativo requer operandos inteiros");
    }

    nextToken();
    Token *rightType = unaryExpr(scope, currentClassName);

    if (!isIntegerType(rightType)) {
      addSemanticError("Operador multiplicativo requer operandos inteiros");
    }

    leftType = new Token(RESERVED, "int");
  }
  return leftType;
}

// unaryExpr -> ! unaryExpr | primaryExpr
Token *Parser::unaryExpr(SymbolTable *scope, const string &currentClassName) {
  if (currentToken->lexeme == "!") {
    nextToken();
    Token *exprType = unaryExpr(scope, currentClassName);

    if (!isBooleanType(exprType)) {
      addSemanticError("Operador ! requer operando booleano");
    }

    return new Token(RESERVED, "boolean");
  } else {
    return primaryExpr(scope, currentClassName);
  }
}

// primaryExpr -> INTEGER_LITERAL | true | false | ID | this
//              | new int [ Expression ] | new ID () | ( Expression )
//              | primaryExpr [ Expression ]
//              | primaryExpr . length
//              | primaryExpr . ID ( (ExpressionsList)? )
Token *Parser::primaryExpr(SymbolTable *scope, const string &currentClassName) {
  Token *type = nullptr;

  if (currentToken->name == INTEGER_LITERAL) {
    type = new Token(RESERVED, "int");
    match(INTEGER_LITERAL);
  } else if (currentToken->lexeme == "true" ||
             currentToken->lexeme == "false") {
    type = new Token(RESERVED, "boolean");
    nextToken();
  } else if (currentToken->lexeme == "this") {

    type = new Token(ID, currentClassName);
    match("this");
  } else if (currentToken->name == ID) {

    string idName = currentToken->lexeme;
    match(ID);
    STEntry *entry = scope->get(idName);
    if (entry == nullptr) {
      addSemanticError("Identificador nao declarado: " + idName);
      type = new Token(UNDEF, "undefined");
    } else {
      type = new Token(ID, entry->type);
    }
  } else if (currentToken->lexeme == "new") {
    nextToken();
    if (currentToken->lexeme == "int") {
      nextToken();
      match("[");
      type = expression(scope, currentClassName);
      if (!isIntegerType(type)) {
        addSemanticError("Tamanho do array deve ser inteiro");
      }
      match("]");
      type = new Token(RESERVED, "int[]");
    } else {
      string className = currentToken->lexeme;
      match(ID);

      // Verifica se a classe existe
      STEntry *classEntry = st->get(className);
      if (classEntry == nullptr || !classEntry->reserved) {
        addSemanticError("Classe nao declarada: " + className);
      }

      match("(");
      match(")");
      type = new Token(ID, className);
    }
  } else if (currentToken->lexeme == "(") {
    nextToken();
    type = expression(scope, currentClassName);
    match(")");
  } else {
    syntaxError("o inicio de uma expressao...");
    return new Token(UNDEF, "undefined");
  }
  while (true) {
    if (currentToken->lexeme == "[") {
      nextToken();

      if (type->lexeme.find("[]") == string::npos) {
        addSemanticError("Acesso a array em variavel nao-array");
      }

      Token *indexType = expression(scope, currentClassName);
      if (!isIntegerType(indexType)) {
        addSemanticError("Indice de array deve ser inteiro");
      }

      match("]");

      if (type->lexeme == "int[]") {
        type = new Token(RESERVED, "int");
      } else {

        string baseType = type->lexeme.substr(0, type->lexeme.find("[]"));
        type = new Token(ID, baseType);
      }
    } else if (currentToken->lexeme == ".") {
      nextToken();
      if (currentToken->lexeme == "length") {
        nextToken();

        if (type->lexeme.find("[]") == string::npos) {
          addSemanticError("Acesso a .length em variavel nao-array");
        }

        type = new Token(RESERVED, "int");
      } else {
        string methodName = currentToken->lexeme;
        match(ID);

        string objectTypeName = type->lexeme;
        if (objectTypeName == "this") {
          objectTypeName = currentClassName;
        }

        STEntry *classDecl = st->get(objectTypeName);
        STEntry *methodEntry = nullptr;

        if (classDecl != nullptr && classDecl->classScope != nullptr) {
          methodEntry = classDecl->classScope->get(methodName);
        }

        match("(");
        vector<Token *> argTypes;
        if (currentToken->lexeme != ")") {

          argTypes = expressionsList(scope, currentClassName);
        }
        match(")");

        if (classDecl == nullptr || classDecl->classScope == nullptr) {
          addSemanticError("Classe '" + type->lexeme +
                           "' nao declarada ou escopo invalido.");
          type = new Token(UNDEF, "undefined");
        } else if (methodEntry == nullptr || !methodEntry->isMethod) {
          addSemanticError("Metodo '" + methodName +
                           "' nao encontrado na classe '" + type->lexeme +
                           "'.");
          type = new Token(UNDEF, "undefined");
        } else {

          if (argTypes.size() != methodEntry->params.size()) {
            addSemanticError(
                "Numero incorreto de argumentos para o metodo '" + methodName +
                "'. Esperado: " + to_string(methodEntry->params.size()) +
                ", Fornecido: " + to_string(argTypes.size()));
          } else {
            for (size_t i = 0; i < argTypes.size(); ++i) {
              string expectedType = methodEntry->params[i]->type;
              string providedType = argTypes[i]->lexeme;
              if (expectedType != providedType) {
                addSemanticError("Tipo do argumento " + to_string(i + 1) +
                                 " para o metodo '" + methodName +
                                 "' esta incorreto. Esperado: '" +
                                 expectedType + "', Fornecido: '" +
                                 providedType + "'.");
              }
            }
          }

          type = new Token(ID, methodEntry->returnType);
        }
      }
    }

    else {
      break;
    }
  }

  return type;
}
// ExpressionsList -> Expression (, Expression)*
vector<Token *> Parser::expressionsList(SymbolTable *scope,
                                        const string &currentClassName) {
  vector<Token *> argTypes;

  Token *argType = expression(scope, currentClassName);
  argTypes.push_back(argType);

  while (currentToken->lexeme == ",") {
    nextToken();
    argType = expression(scope, currentClassName);
    argTypes.push_back(argType);
  }
  return argTypes;
}
void Parser::printSemanticErrors() const {
  for (const auto &error : semanticErrors) {
    cout << error << endl;
  }
}