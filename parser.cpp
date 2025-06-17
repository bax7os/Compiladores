#include "parser.h"
#include <iostream>


Parser::Parser(Scanner* scanner, SymbolTable* st) {
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
void Parser::syntaxError(const string& expected) {
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
        switch(expectedName) {
            case ID: expectedStr = "um identificador"; break;
            case INTEGER_LITERAL: expectedStr = "um numero inteiro"; break;
            case SEPARATOR: expectedStr = "um separador (ex: '{', ';', ...)"; break;
            case SYSTEM_OUT_PRINTLN: expectedStr = "'System.out.println'"; break;
            case END_OF_FILE: expectedStr = "fim do arquivo"; break;
            case OPERATOR: expectedStr = "um operador (ex: +, -, *, <, &&)"; break; 
            default: expectedStr = "token desconhecido";
        }
        syntaxError(expectedStr);
    }
}

// Compares the current token with the expected lexeme (for reserved words and operators)
void Parser::match(const string& expectedLexeme) {
    if (currentToken->lexeme == expectedLexeme) {
        nextToken();
    } else {
        syntaxError("'" + expectedLexeme + "'");
    }
}

//Starts the analysis
void Parser::parse() {
    program();
    cout << "Analise sintatica concluida com sucesso!" << endl;
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
    SymbolTable* mainScope = new SymbolTable(this->st);
    statement(mainScope);
    delete mainScope;
    match("}");
    match("}");
}

// ClassDeclaration -> class ID (extends ID)? { (VarDeclaration)* (MethodDeclaration)* }
void Parser::classDeclaration(SymbolTable* scope) {
    match("class");
    string className = currentToken->lexeme;
    match(ID);

    // add class name to symbol table of parent scope
    scope->add(new STEntry(new Token(ID, className), true));

    if (currentToken->lexeme == "extends") {
        nextToken();
        match(ID); 
    }
    
    SymbolTable* classScope = new SymbolTable(scope);
    match("{");
    
    while (isType()) {
        varDeclaration(classScope);
    }
    
    while (currentToken->lexeme == "public") {
        methodDeclaration(classScope);
    }
    
    match("}");
    delete classScope;
}

// VarDeclaration -> Type ID;
void Parser::varDeclaration(SymbolTable* scope) {
    type();
    scope->add(new STEntry(new Token(ID, currentToken->lexeme)));
    match(ID);
    match(";");
}

// MethodDeclaration -> public Type ID ( (Params)? ) { (VarDeclaration)* (Statement)* return Expression; }
void Parser::methodDeclaration(SymbolTable* scope) {
    match("public");
    type(); 
    match(ID);
    match("(");

    SymbolTable* methodScope = new SymbolTable(scope);

    if (currentToken->lexeme != ")") { 
        params(methodScope);
    }
    match(")");
    match("{");

    while (isType()) {
        varDeclaration(methodScope);
    }

    while (currentToken->lexeme != "return" && currentToken->lexeme != "}") {
        if (currentToken->lexeme == "{" || currentToken->lexeme == "if" || 
            currentToken->lexeme == "while" || currentToken->name == SYSTEM_OUT_PRINTLN || 
            currentToken->name == ID) 
        {
            statement(methodScope);
        } else {
            syntaxError("um comando ou a palavra-chave 'return'");
            break; 
        }
    }

    match("return");
    expression();
    match(";");
    match("}");
    delete methodScope;
}

// Params -> Type ID (, Type ID)*
void Parser::params(SymbolTable* scope) {
    type();
    scope->add(new STEntry(new Token(ID, currentToken->lexeme)));
    match(ID);

    while (currentToken->lexeme == ",") {
        nextToken();
        type();
        scope->add(new STEntry(new Token(ID, currentToken->lexeme)));
        match(ID);
    }
}

// Helper that verifies if the current token can be the start of an type
bool Parser::isType() {
    if (currentToken->lexeme == "int" || currentToken->lexeme == "boolean") {
        return true;
    }

    if (currentToken->name == ID) {
        STEntry* entry = st->get(currentToken->lexeme);
        if (entry != nullptr && entry->reserved) {
            return true;
        }
    }
    return false;
}

// Type -> int []? | boolean | ID
void Parser::type() {
    if (!isType()) {
        syntaxError("um tipo (int, boolean, ou ID de classe)");
    }

    if (currentToken->lexeme == "int") {
        nextToken();
        if (currentToken->lexeme == "[") {
            nextToken();
            match("]");
        }
    } else { 
        nextToken();
    }
}

// Statement -> { (Statement)* } | if ... | while ... | System.out.println ... | ID ...
void Parser::statement(SymbolTable* scope) {
    if (currentToken->lexeme == "{") {
        nextToken();
        while (currentToken->lexeme != "}") {
            statement(scope);
        }
        match("}");
    } 
    else if (currentToken->lexeme == "if") {
        nextToken();
        match("(");
        expression();
        match(")");
        statement(scope);
        match("else");
        statement(scope);
    } 
    else if (currentToken->lexeme == "while") {
        nextToken();
        match("(");
        expression();
        match(")");
        statement(scope);
    } 
    else if (currentToken->name == SYSTEM_OUT_PRINTLN) {
        nextToken();
        match("(");
        expression();
        match(")");
        match(";");
    } 
    else if (currentToken->name == ID) {
        match(ID); 
        if (currentToken->lexeme == "=") { 
            match("=");
            expression();
            match(";");
        } else if (currentToken->lexeme == "[") { 
            match("[");
            expression();
            match("]");
            match("=");
            expression();
            match(";");
        } else {
            syntaxError("um '=' ou '[' apos o identificador em um comando de atribuicao");
        }
    } 
    else {
        syntaxError("um comando ('{', 'if', 'while', 'System.out.println' ou um ID de variavel)");
    }
}

// expression -> comparisonExpr ( && comparisonExpr )*
void Parser::expression() {
    comparisonExpr();
    while (currentToken->lexeme == "&&") {
        nextToken();
        comparisonExpr();
    }
}

// comparisonExpr -> additiveExpr ( ( < | > | == | != ) additiveExpr )?
void Parser::comparisonExpr() {
    additiveExpr();
    if (currentToken->lexeme == "<" || currentToken->lexeme == ">" || 
        currentToken->lexeme == "==" || currentToken->lexeme == "!=") {
        nextToken();
        additiveExpr();
    }
}

// additiveExpr -> multiplicativeExpr ( ( + | - ) multiplicativeExpr )*
void Parser::additiveExpr() {
    multiplicativeExpr();
    while (currentToken->lexeme == "+" || currentToken->lexeme == "-") {
        nextToken();
        multiplicativeExpr();
    }
}

// multiplicativeExpr -> unaryExpr ( ( * | / ) unaryExpr )*
void Parser::multiplicativeExpr() {
    unaryExpr();
    while (currentToken->lexeme == "*" || currentToken->lexeme == "/") {
        nextToken();
        unaryExpr();
    }
}

// unaryExpr -> ! unaryExpr | primaryExpr
void Parser::unaryExpr() {
    if (currentToken->lexeme == "!") {
        nextToken();
        unaryExpr();
    } else {
        primaryExpr();
    }
}

// primaryExpr -> INTEGER_LITERAL | true | false | ID | this
//              | new int [ Expression ] | new ID () | ( Expression )
//              | primaryExpr [ Expression ]
//              | primaryExpr . length
//              | primaryExpr . ID ( (ExpressionsList)? )
void Parser::primaryExpr() {
   
    if (currentToken->name == INTEGER_LITERAL) {
        match(INTEGER_LITERAL);
    } else if (currentToken->lexeme == "true" || currentToken->lexeme == "false") {
        nextToken();
    } else if (currentToken->lexeme == "this") {
        match("this");
    } else if (currentToken->name == ID) {
        match(ID);
    } else if (currentToken->lexeme == "new") {
        nextToken();
        if (currentToken->lexeme == "int") { // new int [expr]
            nextToken();
            match("[");
            expression();
            match("]");
        } else { // new ID()
            match(ID);
            match("(");
            match(")");
        }
    } else if (currentToken->lexeme == "(") {
        nextToken();
        expression();
        match(")");
    } else {
        syntaxError("o inicio de uma expressao (numero, 'true', 'false', 'this', 'new', '(', ou um ID)");
    }

 
    while (true) {
        if (currentToken->lexeme == "[") { // Array access: ... [ expr ]
            nextToken();
            expression();
            match("]");
        } else if (currentToken->lexeme == ".") {
            nextToken();
            if (currentToken->lexeme == "length") { // Length acess: ... .length
                nextToken();
            } else { // Method call: ... .id( (exprList)? )
                match(ID);
                match("(");
                if (currentToken->lexeme != ")") {
                    expressionsList();
                }
                match(")");
            }
        } else {
            break; 
        }
    }
}

// ExpressionsList -> Expression (, Expression)*
void Parser::expressionsList() {
    expression();
    while (currentToken->lexeme == ",") {
        nextToken();
        expression();
    }
}