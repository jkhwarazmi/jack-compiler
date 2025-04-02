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

// Returns first non-whitespace character
int eatWhitespace()
{
  int c;

  while (1)
  {
    // Consume whitespace
    c = fgetc(file);
    while (c != EOF && isspace(c))
    {
      if (c == '\n')
      {
        line++;
      }
      c = fgetc(file);
    }

    // No comment so return character
    if (c != '/')
    {
      return c;
    }

    // Consume comments
    c = fgetc(file);
    if (c == '/')
    {
      while (c != EOF && c != '\n')
      {
        c = fgetc(file);
      }
      if (c == '\n')
      {
        line++;
      }
      else if (c == EOF)
      {
        return c;
      }
    }
    // Multi-line comments
    else if (c == '*')
    {
      int prev = 0;
      c = fgetc(file);
      while (c != EOF)
      {
        if (prev == '*' && c == '/')
        {
          break;
        }
        if (c == '\n')
        {
          line++;
        }
        prev = c;
        c = fgetc(file);
      }
      if (c == EOF)
      {
        return EofInCom;
      }
    }
    // Differentiate between division and comment
    else
    {
      ungetc(c, file);
      return '/';
    }
  }  
  return c;
}

Token buildToken() 
{
  // Remove whitespace and comments
  int c = eatWhitespace();
  if (c == EOF)
  {
    strcpy(t.lx, "End of File");
    t.tp = EOFile;
    t.ln = line;
    return t;
  }
  else if (c == EofInCom)
  {
    t.tp = ERR;
    strcpy(t.lx, "Error: unexpected eof in comment");
    t.ec = EofInCom;
    t.ln = line;
    return t;
  }

  char tmp[128];
  int i = 0;

  // Check for string literals
  if (c == '"')
  {
    c = fgetc(file);
    while (c != '"' && c != EOF && c != '\n')
    {
      tmp[i] = c;
      i++;
      c = fgetc(file);
    }
    if (c == '\n')
    {
      t.tp = ERR;
      strcpy(t.lx, "Error: new line in string constant");
      t.ec = NewLnInStr;
    }
    else if (c == EOF)
    {
      t.tp = ERR;
      strcpy(t.lx, "Error: unexpected eof in string constant");
      t.ec = EofInStr;
    }
    else {
      t.tp = STRING;
      tmp[i] = '\0';
      strcpy(t.lx, tmp);
    }
    t.ln = line;
    return t;
  }
  // Check for keywords and identifiers
  if (isalpha(c) || c == '_')
  {
    while (c != EOF && (isalpha(c) || c == '_' || isdigit(c)))
    {
      tmp[i] = c;
      i++;
      c = fgetc(file);
    }
    tmp[i] = '\0';
    strcpy(t.lx, tmp);
    ungetc(c, file);

    if (isKeyword(tmp))
    {
      t.tp = RESWORD;
    }
    else
    {
      t.tp = ID;
    }
    t.ln = line;
    return t;
  }
  else if (isdigit(c))
  {
    while (c != EOF && isdigit(c))
    {
      tmp[i] = c;
      i++;
      c = fgetc(file);
    }
    tmp[i] = '\0';
    strcpy(t.lx, tmp);
    ungetc(c, file);

    t.tp = INT;
    t.ln = line;
    return t;
  }
  else
  {
    if (isSymbol(c))
    {
      tmp[i] = c;
      tmp[i + 1] = '\0';
      strcpy(t.lx, tmp);
      t.tp = SYMBOL;
    }
    else
    {
      t.tp = ERR;
      strcpy(t.lx, "Error: illegal symbol in source file");
      t.ec = IllSym;
    }
    t.ln = line;
    return t;
  }
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
  if (tokenReady)
  {
    tokenReady = 0;
    return t;
  }
  t = buildToken();
  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  if (tokenReady)
  {
    return t;
  }
  t = buildToken();
  tokenReady = 1;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  if (file) {
    fclose(file);
    file = NULL;
  }
	return 0;
}

// do not remove the next line
#ifndef TEST
// do not remove the next line
#endif
