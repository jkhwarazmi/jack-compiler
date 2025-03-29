#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbols.h"

ProgramSymbol p[100]; // Program symbol table
int plen = 0; // Number of classes in the program symbol table

ClassSymbol *c = p[0].table; // Class symbol table
int clen = 0; // Number of items in the class symbol table

SubSymbol *s = p[0].table[0].table; // Subroutine symbol table
int slen = 0; // Number of items in the subroutine symbol table

// Reset the symbol tables
void initSymbolTable() {
  plen = 0;
  clen = 0;
  slen = 0;

  c = p[0].table;
  s = c[0].table;
}

// Inserts a new class and returns its index, -1 if it already exists
int insertP(char *name) {
  if (findP(name) != -1) {
    return -1;
  }

  strcpy(p[plen].name, name);
  c = p[plen].table;
  s = c[0].table;
  plen++;

  c[0].offset = 0; // Reset class symbol table offset
  s[0].offset = 0; // Reset subroutine symbol table offset

  clen = 0; // Reset class symbol table count
  slen = 0; // Reset subroutine symbol table count

  return plen - 1;
}

// Inserts a class symbol and returns its index, -1 if it already exists
int insertC(char *name, char *type, char *kind) {
  if (findC(name) != -1) {
    return -1;
  }

  strcpy(c[clen].name, name);
  strcpy(c[clen].type, type);
  strcpy(c[clen].kind, kind);

  // Calculate the offset based on the kind of symbol
  if (!(strcmp(kind, "static") && strcmp(kind, "field"))) {
    c[clen].offset = 0;
    int i = clen - 1;

    while (i >= 0) {
      if (!strcmp(c[i].kind, c[clen].kind)) {
        c[clen].offset++;
      }
      i--;
    }
  // Have a subroutine, go to the subroutine table
  } else {
    s = c[clen].table;
    slen = 0;
    s[slen].offset = 0;

    // Static functions have no implicit this argument
    if (strcmp(kind, "function")) {
      strcpy(s[slen].name, "this");
      strcpy(s[slen].type, p[plen - 1].name);
      strcpy(s[slen].kind, "argument");
      slen++;
    }
  }

  clen++;
  return clen - 1;
}

// Inserts a subroutine symbol and returns its index, -1 if it already exists
int insertS(char *name, char *type, char *kind) {
  if (findS(name) != -1) {
    return -1;
  }

  strcpy(s[slen].name, name);
  strcpy(s[slen].type, type);
  strcpy(s[slen].kind, kind);
  s[slen].offset = 0;

  int i = slen - 1;
  while (i >= 0) {
    if (!strcmp(s[i].kind, s[slen].kind)) {
      s[slen].offset++;
    }
    i--;
  }

  slen++;
  return slen - 1;
}

// Returns the index of the class in the table or -1 if not found
int findP(char *name) {
  for (int i = 0; i < plen; i++) {
    if (!strcmp(p[i].name, name)) {
      return i;
    }
  }
  return -1;
}

// Returns the index of the symbol in the table or -1 if not found
int findC(char *name) {
  for (int i = 0; i < clen; i++) {
    if (!strcmp(c[i].name, name)) {
      return i;
    }
  }
  return -1;
}

// Returns the index of the symbol in the table or -1 if not found
int findS(char *name) {
  for (int i = 0; i < slen; i++) {
    if (!strcmp(s[i].name, name)) {
      return i;
    }
  }
  return -1;
}

// Returns the offset of the symbol in the class table based on its index
int getCOffset(int index) {
  return c[index].offset;
}

// Returns the offset of the symbol in the subroutine table based on its index
int getSOffset(int index) {
  return s[index].offset;
}

char *getClassName() {
  return p[plen - 1].name;
}

// Returns the kind of the symbol based on its index
char *getKindC(int index) {
  return c[index].kind;
}

// Returns the kind of the symbol based on its index
char *getKindS(int index) {
  return s[index].kind;
}

// Returns the type of the symbol based on its index
char *getTypeC(int index) {
  return c[index].type;
}

// Returns the type of the symbol based on its index
char *getTypeS(int index) {
  return s[index].type;
}