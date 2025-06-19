# MiniJava Compiler

This project is a compiler for MiniJava, a subset of the Java programming language. It is implemented in C++ and performs a three-stage compilation process: lexical, syntax, and semantic analysis.

The compiler is designed to validate MiniJava source code, ensuring not only that the program's structure is grammatically correct but also that the code is semantically valid. This includes type checking, scope resolution, and validation of variable and method declarations.

## Structure

The compiler's architecture is modular, with distinct components for each analysis phase.

- **`main.cpp`**: The program's entry point. It is responsible for reading the source file from the command line, initializing the global symbol table with reserved words, and orchestrating the execution of the analysis phases.

- **`scanner.h` & `scanner.cpp`**: This is the **lexical analyzer**. The `Scanner` class reads the source code character by character and groups them into a stream of tokens. It identifies keywords, identifiers, integer literals, operators, and separators. It also handles the removal of whitespace and comments (both single-line `//` and block `/* */`).

- **`parser.h` & `parser.cpp`**: This is the **syntax analyzer and semantic analyzer**. The `Parser` class takes the stream of tokens from the scanner and verifies if it conforms to the MiniJava grammar rules. Using a multi-pass pre-scan approach, the parser now performs a complete, syntax-directed semantic analysis. Its responsibilities include: Validating the grammatical structure of the code; Orchestrating all semantic checks; Managing the creation and lifecycle of scopes (global, class, method).

- **`token.h`**: This file defines the `Token` class and an `enum` of all possible token types (`ID`, `INTEGER_LITERAL`, `RESERVED`, etc.). Each token object stores its type, the actual text (lexeme), and an optional attribute.

- **`symboltable.h` & `symboltable.cpp`**: This implements the **Symbol Table**, a critical data structure for managing identifiers (like variables, classes, and method names). It is designed as a hierarchy of tables to correctly handle nested scopes. This allows the compiler to track where an identifier is declared and prevent naming conflicts.

- **`stentry.h` & `stentry.cpp`**: This defines the structure for entries that are stored in the symbol table. Each `STEntry` contains a pointer to a `Token` and a boolean flag to mark if it's a reserved word, which is important for the parser to distinguish types from identifiers.

## MiniJava Grammar

The parser validates the source code against the following formal grammar. This grammar defines the rules for constructing a valid MiniJava program.

1.  **Program** → `MainClass` (`ClassDeclaration`)\* `EOF`
2.  **MainClass** → `class` `ID` `{` `public` `static` `void` `main` (`String` `[` `]` `ID`) `{` `Statement` `}` `}`
3.  **ClassDeclaration** → `class` `ID` (`extends` `ID`)? `{` (`VarDeclaration`)_ (`MethodDeclaration`)_ `}`
4.  **VarDeclaration** → `Type` `ID` `;`
5.  **MethodDeclaration** → `public` `Type` `ID` `(` (`Params`)? `)` `{` (`VarDeclaration`)_ (`Statement`)_ `return` `Expression` `;` `}`
6.  **Params** → `Type` `ID` (`,` `Type` `ID`)\*
7.  **Type** → `int` (`[` `]`)? | `boolean` | `ID`
8.  **Statement** →
    - `{` (`Statement`)\* `}`
    - `if` `(` `Expression` `)` `Statement` `else` `Statement`
    - `while` `(` `Expression` `)` `Statement`
    - `System.out.println` `(` `Expression` `)` `;`
    - `ID` (`[` `Expression` `]`)? `=` `Expression` `;`
9.  **Expression** →
    - `Expression` `Op` `Expression`
    - `Expression` `[` `Expression` `]`
    - `Expression` `.` `length`
    - `Expression` `.` `ID` `(` (`ExpressionsList`)? `)`
    - `INTEGER_LITERAL`
    - `true`
    - `false`
    - `ID`
    - `this`
    - `new` `int` `[` `Expression` `]`
    - `new` `ID` `(` `)`
    - `!` `Expression`
    - `(` `Expression` `)`
10. **Op** → `&&` | `<` | `>` | `==` | `!=` | `+` | `-` | `*` | `/`
11. **ExpressionsList** → `Expression` (`,` `Expression`)\*

---

## How to Run

To compile and run the MiniJava compiler, you need a C++ compiler like g++.

### 1. Compilation

Open your terminal, navigate to the directory containing all the `.cpp` and `.h` files, and run the following command to compile the project. This will create an executable file named `mj_compiler`.

```bash
g++ -o mj_compiler *.cpp
```

### 2. Execution

Once compiled, you can run the compiler by passing it a MiniJava source file (.mj) as a command-line argument

```bash
./mj_compiler <your_file.mj>
```

If the code in **`<your_file.mj>`** is syntactically correct, the program will output:
**`Syntax analyzer and semantic analyzer completed!`**

If there are any lexical or syntax errors, the compiler will stop and report the error with its line number.
