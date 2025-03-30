#ifndef COMPILER_H
#define COMPILER_H

#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"

int InitCompiler ();
ParserInfo compile (char *dir_name);
int StopCompiler();

// Returns 1 if the file is a Jack file, 0 otherwise
int isJackFile(char *filename);

// Prints the error message
void print(ParserInfo pi);

#endif

