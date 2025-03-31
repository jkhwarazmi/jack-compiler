#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include "compiler.h"

// Returns 1 if the file is a Jack file, 0 otherwise
int isJackFile(char *filename) {
  char *ext = strrchr(filename, '.');
  return ext && strcmp(ext, ".jack") == 0;
}

int InitCompiler ()
{
  initSymbolTable();
	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;

	// write your code below
  DIR *d;
  struct dirent *entry;
  char file_name[1024];
  FILE *f;

  d = opendir(dir_name);
  if (!d) {
    printf("Could not open directory\n");
    exit(1);
  }

  while ((entry = readdir(d)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		} else if (isJackFile(entry->d_name)) {
      snprintf(file_name, sizeof(file_name), "%s/%s", dir_name, entry->d_name);
      if (!InitLexer(file_name)) {
        exit(1);
      }
			p = Parse();
			if (p.er != none) {
				return p;
			}
    }
  }

  if ((p = checkClasses()).er != none) {
    return p;
  }

	if ((p = checkClassCalls()).er!= none) {
		return p;
	}
  
  closedir(d);
	return p;
}

int StopCompiler ()
{
	StopParser();
	return 1;
}

void printError(ParserInfo pi) {
  switch(pi.er) {
		case none:
			break;
		case lexerErr:
			printf("Line %i at or near %s: lexer error\n", pi.tk.ln, pi.tk.lx);
			break;
		case classExpected:
			printf("Line %i at or near %s: class expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case idExpected:
			printf("Line %i at or near %s: identifier expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case openBraceExpected:
			printf("Line %i at or near %s: { expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case closeBraceExpected:
			printf("Line %i at or near %s: } expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case memberDeclarErr:
			printf("Line %i at or near %s: class member declaration must begin with static, field, constructor , function , or method\n", pi.tk.ln, pi.tk.lx);
			break;
		case classVarErr:
			printf("Line %i at or near %s: class variables must begin with field or static\n", pi.tk.ln, pi.tk.lx);
			break;
		case illegalType:
			printf("Line %i at or near %s: a type must be int, char, boolean, or identifier\n", pi.tk.ln, pi.tk.lx);
			break;
		case semicolonExpected:
			printf("Line %i at or near %s: ; expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case subroutineDeclarErr:
			printf("Line %i at or near %s: subrouting declaration must begin with constructor, function, or method\n", pi.tk.ln, pi.tk.lx);
			break;
		case openParenExpected:
			printf("Line %i at or near %s: ( expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case closeParenExpected:
			printf("Line %i at or near %s: ) expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case closeBracketExpected:
			printf("Line %i at or near %s: ] expected\n", pi.tk.ln, pi.tk.lx);
			break;
		case equalExpected:
			printf("Line %i at or near %s: = expected\n", pi.tk.ln, pi.tk.lx);
			break;
			case undecIdentifier:
			printf("Line %i at or near %s: undeclared identifier\n", pi.tk.ln, pi.tk.lx);
			break;
		case redecIdentifier:
			printf("Line %i at or near %s: redeclaration of identifier\n", pi.tk.ln, pi.tk.lx);
			break;
		default:
			printf("Line %i at or near %s: syntax error\n", pi.tk.ln, pi.tk.lx);
			break;
	}
}

#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Pong");
	printError(p);
	StopCompiler ();
	return 1;
}
#endif
