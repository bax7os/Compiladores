#include <iostream>
#include <fstream>
#include <sstream>
#include "parser.h"
#include "scanner.h"
#include "symboltable.h"


// Function for inicializing the global symbol table with the reserved words
// The flag 'true' is crucial, since it is used by the parser to identify
// if an ID is a type (like 'int', 'boolean' or a class name).
void initSymbolTable(SymbolTable* st) {
    st->add(new STEntry(new Token(RESERVED, "boolean"), true));
    st->add(new STEntry(new Token(RESERVED, "class"), true));
    st->add(new STEntry(new Token(RESERVED, "else"), true));
    st->add(new STEntry(new Token(RESERVED, "extends"), true));
    st->add(new STEntry(new Token(RESERVED, "false"), true));
    st->add(new STEntry(new Token(RESERVED, "if"), true));
    st->add(new STEntry(new Token(RESERVED, "int"), true));
    st->add(new STEntry(new Token(RESERVED, "length"), true));
    st->add(new STEntry(new Token(RESERVED, "main"), true));
    st->add(new STEntry(new Token(RESERVED, "new"), true));
    st->add(new STEntry(new Token(RESERVED, "public"), true));
    st->add(new STEntry(new Token(RESERVED, "return"), true));
    st->add(new STEntry(new Token(RESERVED, "static"), true));
    st->add(new STEntry(new Token(RESERVED, "String"), true));
    st->add(new STEntry(new Token(SYSTEM_OUT_PRINTLN, "System.out.println"), true));
    st->add(new STEntry(new Token(RESERVED, "this"), true));
    st->add(new STEntry(new Token(RESERVED, "true"), true));
    st->add(new STEntry(new Token(RESERVED, "void"), true));
    st->add(new STEntry(new Token(RESERVED, "while"), true));
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo.mj>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "Erro: Nao foi possivel abrir o arquivo '" << argv[1] << "'" << endl;
        return 1;
    }

    // Read all file content to a string
    stringstream buffer;
    buffer << file.rdbuf();
    string input = buffer.str();
    file.close();

    SymbolTable* st = new SymbolTable();
    initSymbolTable(st);


    Scanner* scanner = new Scanner(input, st);
    Parser* parser = new Parser(scanner, st);
    parser->parse();


    delete parser;
    delete scanner;
    delete st;

    return 0;
}