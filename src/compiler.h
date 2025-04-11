#ifndef COMPILER_H
#define COMPILER_H

//#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"

int InitCompiler ();
ParserInfo compile (char *dir_name);
int StopCompiler();

// Returns 1 if the file is a Jack file, 0 otherwise
int isJackFile(char *filename);

// Prints the error message
void print(ParserInfo pi);

// Writes the output to the file
void emit(char *s);

// Adds lines to be written to the output
void appendLn(char *line);

// Writes the vm code to the output file
void outputLines();

// Returns the current label and increments it
int getLabel();

// Determines whether the current function is void or not
void setVoid(char *type);

#endif

