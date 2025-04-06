#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbols.h"
#include "compiler.h"

ProgramSymbol p[100]; // Program symbol table
int plen = 0; // Number of classes in the program symbol table

ClassSymbol *c = p[0].table; // Class symbol table
int clen = 0; // Number of items in the class symbol table

SubSymbol *s = p[0].table[0].table; // Subroutine symbol table
int slen = 0; // Number of items in the subroutine symbol table

SubCalls sc[100]; // Subroutine calls table to check later
int sclen = 0; // Number of subroutine calls in the table

SubCalls classes[100];
int cclen = 0;

int statics = 0;

// Reset the symbol tables
void initSymbolTable() {
  plen = 0;
  clen = 0;
  slen = 0;
  sclen = 0;
  cclen = 0;
  statics = 0;

  c = p[0].table;
  s = c[0].table;

  for (int i = 0; i < 100; i++) {
    strcpy(p[i].name, "");
    
    strcpy(c[i].name, "");
    strcpy(c[i].type, "");
    strcpy(c[i].kind, "");
    c[i].offset = 0;

    strcpy(s[i].name, "");
    strcpy(s[i].type, "");
    strcpy(s[i].kind, "");
    s[i].offset = 0;
  }
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

  // Empty both symbol tables
  for (int i = 0; i < 100; i++) {
    strcpy(c[i].name, "");
    strcpy(c[i].type, "");
    strcpy(c[i].kind, "");
    c[i].offset = 0;

    strcpy(s[i].name, "");
    strcpy(s[i].type, "");
    strcpy(s[i].kind, "");
    s[i].offset = 0;
  }

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

    if (!strcmp(kind, "static")) {
      c[clen].offset = statics;
      statics++;
    } else {
      while (i >= 0) {
        if (!strcmp(c[i].kind, c[clen].kind)) {
          c[clen].offset++;
        }
        i--;
      }
    }
  // Have a subroutine, go to the subroutine table
  } else {
    s = c[clen].table;
    slen = 0;
    s[slen].offset = 0;

    // Empty the subroutine symbol table
    for (int i = 0; i < 100; i++) {
      strcpy(s[i].name, "");
      strcpy(s[i].type, "");
      strcpy(s[i].kind, "");
      s[i].offset = 0;
    }

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

// Adds a subroutine call to the table to check after we've filled the symbol table
void checkLater(char *class, char *prop, int line) {
  strcpy(sc[sclen].class, class);
  strcpy(sc[sclen].prop, prop);
  sc[sclen].line = line;
  sclen++;
}

void checkClassLater(char *class, int line) {
  strcpy(classes[cclen].class, class);
  classes[cclen].line = line;
  cclen++;
}

// Checks types to see if they're valid classes
ParserInfo checkClasses() {
  ParserInfo pi;
  pi.tk.tp = 1;
  pi.er = none;

  for (int i = 0; i < cclen; i++) {
    // First check std library
    if (isStd(classes[i].class) != -1) {
      continue;
    }
    
    // Find the class in the program symbol table
    int classIndex = findP(classes[i].class);
    if (classIndex == -1) {
      strcpy(pi.tk.lx, classes[i].class);
      pi.tk.ln = classes[i].line;
      pi.er = undecIdentifier;
      return pi;
    }
  }
  return pi;
}

// Checks the class calls in the table to see if they are valid
ParserInfo checkClassCalls() {
  ParserInfo pi;
  pi.tk.tp = 1;
  pi.er = none;
  clen = 100;

  for (int i = 0; i < sclen; i++) {
    // First check std library
    if (isStd(sc[i].class) != -1) {
      // Check if the method is valid
      if (isStdMethod(sc[i].class, sc[i].prop) == -1) {
        strcpy(pi.tk.lx, sc[i].prop);
        pi.tk.ln = sc[i].line;
        pi.er = undecIdentifier;
        return pi;
      }
      continue;
    }

    // Find the class in the program symbol table
    int classIndex = findP(sc[i].class);
    if (classIndex == -1) {
      strcpy(pi.tk.lx, sc[i].class);
      pi.tk.ln = sc[i].line;
      pi.er = undecIdentifier;
      return pi;
    }

    // Set the class symbol table
    c = p[classIndex].table;

    // Find the property in the class symbol table
    int methodIndex = findC(sc[i].prop);
    if (methodIndex == -1) {
      strcpy(pi.tk.lx, sc[i].prop);
      pi.tk.ln = sc[i].line;
      pi.er = undecIdentifier;
      return pi;
    }
  }
  return pi;
}

// Function to check if a class name is a standard library, returns its index or -1 if not
int isStd(char *className) {
  for (int i = 0; i < STD_LIBRARY_SIZE; i++) {
    if (strcmp(className, STD_LIBRARY[i]) == 0) {
      return i;
    }
  }
  return -1;
}

// Function to check if a method belongs to a specific standard library, returns arguments or -1 if not
int isStdMethod(char *className, char *methodName) {
  int index = isStd(className);
  
  // Check method against the appropriate function list
  switch (index) {
    // Math
    case 0:
      for (int i = 0; i < MATH_FUNCS_SIZE; i++) {
        if (!strcmp(methodName, MATH_FUNCS[i])) {
          setVoid(MATH_FUNCS_RET[i]);
          return MATH_FUNCS_ARGS[i];
        }
      }
      break;
    // Memory
    case 1:
      for (int i = 0; i < MEMORY_FUNCS_SIZE; i++) {
        if (!strcmp(methodName, MEMORY_FUNCS[i])) {
          setVoid(MEMORY_FUNCS_RET[i]);
          return MEMORY_FUNCS_ARGS[i];
        }
      }
      break;
    // Screen
    case 2:
      for (int i = 0; i < SCREEN_FUNCS_SIZE; i++) {
        if (!strcmp(methodName, SCREEN_FUNCS[i])) {
          setVoid(SCREEN_FUNCS_RET[i]);
          return SCREEN_FUNCS_ARGS[i];
        }
      }
      break;
    // Output
    case 3:
      for (int i = 0; i < OUTPUT_FUNCS_SIZE; i++) {
        if (!strcmp(methodName, OUTPUT_FUNCS[i])) {
          setVoid(OUTPUT_FUNCS_RET[i]);
          return OUTPUT_FUNCS_ARGS[i];
        }
      }
      break;
    // Keyboard
    case 4:
      for (int i = 0; i < KEYBOARD_FUNCS_SIZE; i++) {
        if (strcmp(methodName, KEYBOARD_FUNCS[i]) == 0) {
          setVoid(KEYBOARD_FUNCS_RET[i]);
          return KEYBOARD_FUNCS_ARGS[i];
        }
      }
      break;
    // String   
    case 5:
      for (int i = 0; i < STRING_FUNCS_SIZE; i++) {
        if (strcmp(methodName, STRING_FUNCS[i]) == 0) {
          setVoid(STRING_FUNCS_RET[i]);
          return STRING_FUNCS_ARGS[i];
        }
      }
      break;
    // Array
    case 6:
      for (int i = 0; i < ARRAY_FUNCS_SIZE; i++) {
        if (strcmp(methodName, ARRAY_FUNCS[i]) == 0) {
          setVoid(ARRAY_FUNCS_RET[i]);
          return ARRAY_FUNCS_ARGS[i];
        }
      }
      break;
    // Sys  
    case 7:
      for (int i = 0; i < SYS_FUNCS_SIZE; i++) {
        if (strcmp(methodName, SYS_FUNCS[i]) == 0) {
          setVoid(SYS_FUNCS_RET[i]);
          return SYS_FUNCS_ARGS[i];
        }
      }
      break;
  }
  
  return -1;
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

// Get Number of arguments for a method
int getNumArgs(char *name) {
  // Split the string into class and method
  char class[100], method[100];
  sscanf(name, "%[^.].%s", class, method);
  
  // Check if the class is a standard library
  if (isStd(class) != -1) {
    return isStdMethod(class, method);
  }

  // Reset the symbol table
  plen = 100;
  clen = 100;
  slen = 100;

  c = p[findP(class)].table;
  int methodIndex = findC(method);
  setVoid(c[methodIndex].type);
  s = c[methodIndex].table;

  // Count the number of arguments
  int numArgs = 0;
  for (int i = 0; i < slen; i++) {
    if (!strcmp(s[i].kind, "argument")) {
      numArgs++;
    }
  }
  return numArgs;
}

// Get number of local variables for a method
int getNumLocals(char *name) {
  // Split the string into class and method
  char class[100], method[100];
  sscanf(name, "%[^.].%s", class, method);

  // Reset the symbol table
  plen = 100;
  clen = 100;
  slen = 100;

  c = p[findP(class)].table;
  s = c[findC(method)].table;

  // Count the number of local variables
  int numLocals = 0;
  for (int i = 0; i < slen; i++) {
    if (!strcmp(s[i].kind, "local")) {
      numLocals++;
    }
  }
  
  return numLocals;
}