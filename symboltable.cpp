#include "symboltable.h"

//Constructor thet instantiates a new table without specifying a parent table.
SymbolTable::SymbolTable()
{
    parent = 0;
}

//Constructor thet instantiates a new table specifying a parent table.
SymbolTable::SymbolTable(SymbolTable* p)
{
    parent = p;
}

// Try adding a new symbol to the current table. First, check if there is already
// an entry for the lexeme in the current table and, if so, the insertion is not
// perfomed and the method returns false; otherwise, the insertion is performed
// and true is returned.
bool 
SymbolTable::add(STEntry* t)
{
    if (!(symbols.find(t->token->lexeme) == symbols.end()))
        return false;
    //cout << "Inseri";
    symbols.insert({t->token->lexeme, t});
    return true;
}
//Try removing a symbol from the current table. 
bool 
SymbolTable::remove(string name)
{
    return symbols.erase(name) != 0;
}

//Clear the current table.
void 
SymbolTable::clear()
{
    symbols.clear();
}

//Checks if the current table is empty.
bool 
SymbolTable::isEmpty()
{
    return symbols.empty();
}


// Search a entry equivalent to the lexame passed as parameter.
// The search starts in the escope current and goes up in the hierarchy
// until the symbol is not found, possibly reaching the global scope.
// Returns a pointer to the entry found or 0, if the symbol is not present.

STEntry* 
SymbolTable::get(string name)
{
    SymbolTable* table = this;

    auto s = table->symbols.find(name);

    while (s == table->symbols.end())
    {
        table = table->parent;
        if (table == 0)
            return 0;

        s = table->symbols.find(name);
    } 
    
    return s->second;
}

SymbolTable*
SymbolTable::getParent()
{
    return parent;
}
