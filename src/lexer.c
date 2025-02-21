#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

// Define the keywords and symbols
#define NUMKEYWORDS 21
const char *keywords[NUMKEYWORDS] = {
  "class", "constructor", "method", "function",    // Program components
  "int", "boolean", "char", "void",                // Primitive types
  "var", "static", "field",                        // Variable declarations
  "let", "do", "if", "else", "while", "return",    // Statements
  "true", "false", "null",                         // Constants
  "this"                                           // Object reference
};

# define NUMSYMBOLS 19
const char symbols[NUMSYMBOLS] = {
  '(' , ')' , '[', ']', '{', '}', ',', ';', '=', '.',
  '+', '-', '*', '/', '&', '|', '~', '<', '>'
};

FILE *file;
Token t;
int line, tokenReady;

// Loops through the keywords array and checks if the string is a keyword
int isKeyword(char *str)
{
  for (int i = 0; i < NUMKEYWORDS; i++)
  {
    if (!strcmp(str, keywords[i]))
    {
      return 1;
    }
  }
  return 0;
}

// Loops through the symbols array and checks if the character is a symbol
int isSymbol(char c)
{
  for (int i = 0; i < NUMSYMBOLS; i++)
  {
    if (c == symbols[i])
    {
      return 1;
    }
  }
  return 0;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  if ((file = fopen(file_name, "r")))
  {
    tokenReady = 0;
    line = 1;
    strcpy(t.fl, file_name);
    return 1;
  }
  return 0;
}



// Get the next token from the source file
Token GetNextToken ()
{
	Token t;
  t.tp = ERR;
  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
	Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  fclose(file);
	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function

	return 0;
}
// do not remove the next line
#endif
