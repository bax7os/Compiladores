#include "scanner.h"
#include <iostream>

// Constructor
Scanner::Scanner(string input, SymbolTable* st) {
    this->input = input;
    this->st = st;
    this->pos = 0;
    this->line = 1;
}

// Returns the current line
int Scanner::getLine() {
    return this->line;
}
//Reports a lexiacal error and finishes the execution
void Scanner::lexicalError(string msg) {
    cout << "Erro lexico na linha " << this->line << ": " << msg << endl;
    exit(1);
}
//Principal Method that returns the next token
Token* Scanner::nextToken() {
    string lexeme = "";

    // Loop for ignoring spaces, new lines and comments
    while (true) {
        // If we have reached the end of the input
        if (pos >= input.length()) {
            return new Token(END_OF_FILE, "EOF");
        }

        char currentChar = input[pos];
        // Ignore white spaces and tabs
        if (currentChar == ' ' || currentChar == '\t' || currentChar == '\r') {
            pos++;
            continue;
        }

        // Ignore new lines
        if (currentChar == '\n') {
            pos++;
            line++;
            continue;
        }

        // Ignore comments
        if (currentChar == '/') {
            // Block comments /* ... */
            if (pos + 1 < input.length() && input[pos + 1] == '*') {
                pos += 2; 
                while (pos + 1 < input.length() && !(input[pos] == '*' && input[pos + 1] == '/')) {
                    if (input[pos] == '\n') line++;
                    pos++;
                }
                if (pos + 1 >= input.length()) {
                     lexicalError("Comentario de bloco nao fechado.");
                }
                pos += 2; 
                continue;
            } 
            // Line comments // ...
            else if (pos + 1 < input.length() && input[pos + 1] == '/') {
                pos += 2; 
                while (pos < input.length() && input[pos] != '\n') {
                    pos++;
                }
                continue;
            }
        }
        // If we have reached here, it means that the current character is not a space, a tab, a new line or a comment
        break;
    }

    // -- Start of Token Identification  --//

    char currentChar = input[pos];


   // Id (and reserved words)
if (isalpha(currentChar)) {
    lexeme += currentChar;
    pos++;
    while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
        lexeme += input[pos];
        pos++;
    }

    // "System.out.println"
    if (lexeme == "System" && input.substr(pos, 12) == ".out.println") {
        if (pos + 12 >= input.length() || !isalnum(input[pos+12])) {
            pos += 12;
            return new Token(SYSTEM_OUT_PRINTLN, "System.out.println");
        }
    }

    // Reserved words
    if (lexeme == "class" || lexeme == "public" || lexeme == "static" || lexeme == "void" ||
        lexeme == "main" || lexeme == "return" || lexeme == "int" || lexeme == "boolean" ||
        lexeme == "if" || lexeme == "else" || lexeme == "while" || lexeme == "true" || lexeme == "false" ||
        lexeme == "new" || lexeme == "this" || lexeme == "String" || lexeme == "extends") 
    {
        return new Token(RESERVED, lexeme);
    }

    // Otherwise, it's an commun ID
    return new Token(ID, lexeme);
}


    // Integers-Literal
    if (isdigit(currentChar)) {
        lexeme += currentChar;
        pos++;
        while (pos < input.length() && isdigit(input[pos])) {
            lexeme += input[pos];
            pos++;
        }
        return new Token(INTEGER_LITERAL, lexeme);
    }

    // Operators
    if (ispunct(currentChar)) {

        //Operators of 2 characters: &&, ==, !=
        if ((pos + 1) < input.length()) {
            char nextChar = input[pos + 1];
            if (currentChar == '&' && nextChar == '&') {
                pos += 2;
                return new Token(OPERATOR, "&&");
            }
            if (currentChar == '=' && nextChar == '=') {
                pos += 2;
                return new Token(OPERATOR, "==");
            }
            if (currentChar == '!' && nextChar == '=') {
                pos += 2;
                return new Token(OPERATOR, "!=");
            }
        }
        //Operators and Separators of 1 character
        lexeme += currentChar;
        pos++;
        switch(currentChar) {
            case '<':
            case '>':
            case '+':
            case '-':
            case '*':
            case '/':
            case '=':
            case '!':
                return new Token(OPERATOR, lexeme);
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case ';':
            case '.':
            case ',':
                return new Token(SEPARATOR, lexeme);
        }
    }
    // If reaches here, it's an invalid character
    lexicalError("Caractere invalido: '" + string(1, currentChar) + "'");

    return new Token(UNDEF);
}