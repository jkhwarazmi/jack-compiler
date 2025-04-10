#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include "compiler.h"

FILE *f = NULL;
// Flag to indicate if the current function is void or not
int voidFunction = 0; 

// Lines to be output
char lines[2048][1024];
int lineCount = 0;
int labels = 0;

// Returns 1 if the file is a Jack file, 0 otherwise
int isJackFile(char *filename) {
  char *ext = strrchr(filename, '.');
  return ext && strcmp(ext, ".jack") == 0;
}

// Adds a line to the output
void appendLn(char *line) {
	strcpy(lines[lineCount], line);
	lineCount++;
}

// Returns the current label and increments it
int getLabel() {
	labels++;
	return labels - 1;
}

// Write the vm code to the output file
void outputLines() {
	for (int i = 0; i < lineCount; i++) {
		// Create new output file
		if (!strncmp(lines[i], "FILENAME:", 9)) {
			if (f) {
				fclose(f);
				f = NULL;
				labels = 0;
			}

			char *filename = lines[i] + 9;
			f = fopen(filename, "w");

			if (f == NULL) {
				printf("Error creating output file: %s\n", filename);
				exit(1);
			}
		} else if (f) {
			if (!strncmp(lines[i], "function ", 9)) {
				int isConstructor = 0;
				int isMethod = 0;

				char *end = strrchr(lines[i], '.');
				char *class = strndup(lines[i] + 9, end - (lines[i] + 9));
  			if (end && !strcmp(end, ".new")) {
					isConstructor = 1;
				} else if (end && strcmp("function", getKindSub(lines[i] + 9))) {
					isMethod = 1;
				}

				int locals = getNumLocals(lines[i] + 9);
				char num[20];
				sprintf(num, " %d\n", locals);
				strcat(lines[i], num);
				fprintf(f, "%s", lines[i]);

				if (isConstructor) {
					fprintf(f, "push constant %d\n", getNumFields(class));
					fprintf(f, "call Memory.alloc 1\n");
					fprintf(f, "pop pointer 0\n");
				} else if (isMethod) {
					fprintf(f, "push argument 0\n");
					fprintf(f, "pop pointer 0\n");
				}
			} else if (!strncmp(lines[i], "call ", 5)) {
				int args = getNumArgs(lines[i] + 5);
				char num[20];
				sprintf(num, " %d\n", args);
				strcat(lines[i], num);
				fprintf(f, "%s", lines[i]);

				// Pop void function returns to temp
				if (voidFunction) {
					fprintf(f, "pop temp 0\n");
				}
		  } else if (!strncmp(lines[i], "CHECKTHIS:", 10)) {
				if (strcmp("function", getKindSub(lines[i] + 10))) {
					fprintf(f, "push pointer 0\n");
				}
			} else if (lines[i][0] != '\0') {
				// Write the line to the output file
				fprintf(f, "%s\n", lines[i]);
			}
		}
	}
	if (f) {
		fclose(f);
		f = NULL;
	}
}

void setVoid(char *type) {
	if (!strcmp(type, "void")) {
		voidFunction = 1;
	} else {
		voidFunction = 0;
	}
}

int InitCompiler ()
{
  initSymbolTable();
	for (int i = 0; i < 1024; i++) {
		lines[i][0] = '\0';
	}
	lineCount = 0;
	labels = 0;
	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;

	// write your code below
  DIR *d;
  struct dirent *entry;
  char file_name[1024];

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

			// Create output filename
			char output[256];
			int len = strlen(file_name) - 5;
			strncpy(output, file_name, strlen(file_name) - 5);
			output[len] = '\0';
			strcat(output, ".vm");

			strcpy(lines[lineCount], "FILENAME:");
			strcat(lines[lineCount], output);
			lineCount++;
			
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
	outputLines();
	StopCompiler();
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
