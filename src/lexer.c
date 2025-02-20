#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE



// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
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
