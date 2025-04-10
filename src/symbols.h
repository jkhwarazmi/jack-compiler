#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below
// JACK Standard Library
static const char *STD_LIBRARY[] = {
  "Math", "Memory", "Screen", "Output", "Keyboard", "String", "Array", "Sys"
};

// Math functions
static const char *MATH_FUNCS[] = {
  "abs", "multiply", "divide", "min", "max", "sqrt"
};

// Math functions arguments
static const int MATH_FUNCS_ARGS[] = {
  1, 2, 2, 2, 2, 1
};

// Math functions return types
static char *MATH_FUNCS_RET[] = {
  "int", "int", "int", "int", "int", "int"
};

// Memory functions
static const char *MEMORY_FUNCS[] = {
  "peek", "poke", "alloc", "deAlloc"
};

// Memory functions arguments
static const int MEMORY_FUNCS_ARGS[] = {
  1, 2, 1, 1
};

// Memory functions return types
static char *MEMORY_FUNCS_RET[] = {
  "int", "void", "Array", "void"
};

// Screen functions
static const char *SCREEN_FUNCS[] = {
  "clearScreen", "setColor", "drawPixel", "drawLine", "drawRectangle", "drawCircle"
};

// Screen functions arguments
static const int SCREEN_FUNCS_ARGS[] = {
  0, 1, 2, 4, 4, 3
};

// Screen functions return types
static char *SCREEN_FUNCS_RET[] = {
  "void", "void", "void", "void", "void", "void"
};

// Output functions
static const char *OUTPUT_FUNCS[] = {
  "init", "moveCursor", "printChar", "printString", "printInt", "println", "backSpace"
};

// Output functions arguments
static const int OUTPUT_FUNCS_ARGS[] = {
  0, 2, 1, 1, 1, 0, 0
};

// Output functions return types
static char *OUTPUT_FUNCS_RET[] = {
  "void", "void", "void", "void", "void", "void", "void"
};

// Keyboard functions
static const char *KEYBOARD_FUNCS[] = {
  "keyPressed", "readChar", "readLine", "readInt"
};

// Keyboard functions arguments
static const int KEYBOARD_FUNCS_ARGS[] = {
  0, 0, 1, 1
};

// Keyboard functions return types
static char *KEYBOARD_FUNCS_RET[] = {
  "char", "char", "String", "int"
};

// String functions
static const char *STRING_FUNCS[] = {
  "new", "dispose", "length", "charAt", "setCharAt", "appendChar", "eraseLastChar", 
  "intValue", "setInt", "newLine", "backSpace", "doubleQuote"
};

// String functions arguments
static const int STRING_FUNCS_ARGS[] = {
  1, 0, 0, 1, 2, 2, 0, 0, 1, 0, 0, 0
};

// String functions return types
static char *STRING_FUNCS_RET[] = {
  "String", "void", "int", "char", "void", "String", "void", "int", "void", "char", "char", "char"
};

// Array functions
static const char *ARRAY_FUNCS[] = {
  "new", "dispose"
};

// Array functions arguments
static const int ARRAY_FUNCS_ARGS[] = {
  1, 0
};

// Array functions return types
static char *ARRAY_FUNCS_RET[] = {
  "Array", "void"
};

// System functions
static const char *SYS_FUNCS[] = {
  "halt", "error", "wait"
};

// System functions arguments
static const int SYS_FUNCS_ARGS[] = {
  0, 1, 1
};

// System functions return types
static char *SYS_FUNCS_RET[] = {
  "void", "void", "void"
};

// Sizes
static const int STD_LIBRARY_SIZE = sizeof(STD_LIBRARY) / sizeof(STD_LIBRARY[0]);
static const int MATH_FUNCS_SIZE = sizeof(MATH_FUNCS) / sizeof(MATH_FUNCS[0]);
static const int MEMORY_FUNCS_SIZE = sizeof(MEMORY_FUNCS) / sizeof(MEMORY_FUNCS[0]);
static const int SCREEN_FUNCS_SIZE = sizeof(SCREEN_FUNCS) / sizeof(SCREEN_FUNCS[0]);
static const int OUTPUT_FUNCS_SIZE = sizeof(OUTPUT_FUNCS) / sizeof(OUTPUT_FUNCS[0]);
static const int KEYBOARD_FUNCS_SIZE = sizeof(KEYBOARD_FUNCS) / sizeof(KEYBOARD_FUNCS[0]);
static const int STRING_FUNCS_SIZE = sizeof(STRING_FUNCS) / sizeof(STRING_FUNCS[0]);
static const int ARRAY_FUNCS_SIZE = sizeof(ARRAY_FUNCS) / sizeof(ARRAY_FUNCS[0]);
static const int SYS_FUNCS_SIZE = sizeof(SYS_FUNCS) / sizeof(SYS_FUNCS[0]);

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

typedef struct {
  char class[128]; // name of the class
  char prop[128]; // name of the method or variable
  int line; // line number of the call
} SubCalls;

// Reset the symbol tables
void initSymbolTable();

// Inserts values into the relevant symbol table and returns the index of the symbol, -1 if it already exists
int insertP(char *name);
int insertC(char *name, char *type, char *kind);
int insertS(char *name, char *type, char *kind);

// Adds a subroutine call to the table to check later
void checkLater(char *name, char *method, int line);

void checkClassLater(char *name, int line);

// Checks class names for being in the table
ParserInfo checkClasses();

// Checks if the subroutine call is valid and returns 0 if it is, 0 otherwise
ParserInfo checkClassCalls();

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

// Returns index in the std library if a class is standard, -1 if not
int isStd(char *name);

// Returns index in the std library if a method is standard, -1 if not
int isStdMethod(char *name, char *method);

// Get Number of arguments for a method
int getNumArgs(char *name);

// Get number of local variables for a method
int getNumLocals(char *name);

// Get number of fields for a class
int getNumFields(char *name);

// Get the kind of a subroutine based on its class and function name
char *getKindSub(char *name);

#endif
