#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below
typedef struct {
  char name[128]; // name of the variable
  char type[128]; // type of the variable (int, char, boolean or class name)
  char kind[20]; // Kind of variable (local, argument)
  int offset; // Offset of the variable in its address space
} SubSymbol;

typedef struct {
  char name[128]; // name of the variable
  char type[128]; // type of the variable (int, char, boolean or class name)
  char kind[20]; // kind of variable (method, function, constructor, static, field)
  int offset; // Offset of the variable in the class, we don't care about subroutines
  SubSymbol table[100]; // Subroutine symbol table if kind is method, function, or constructor
} ClassSymbol;

typedef struct {
  char name[128]; // name of the class
  ClassSymbol table[100]; // Class symbol table
} ProgramSymbol;

// Reset the symbol tables
void initSymbolTable();

// Inserts values into the relevant symbol table and returns the index of the symbol, -1 if it already exists
int insertP(char *name);
int insertC(char *name, char *type, char *kind);
int insertS(char *name, char *type, char *kind);

// Returns the name of the current class
char *getClassName();

// Returns the index of the symbol in the table or -1 if not found
int findP(char* name);
int findC(char* name);
int findS(char* name);

// Returns the offset of the symbol in the table based on its index
int getCOffset(int index);
int getSOffset(int index);

// Get the kind of the symbol based on its index
char *getKindC(int index);
char *getKindS(int index);

// Gets the type of the symbol based on its index
char *getTypeC(int index);
char *getTypeS(int index);

#endif
