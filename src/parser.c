#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "compiler.h"


// you can declare prototypes of parser functions below
ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo arithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();


int InitParser (char* file_name)
{
	return InitLexer(file_name);
}

ParserInfo Parse ()
{
	ParserInfo pi;

	// implement the function
	pi = classDeclar();
	return pi;
}


int StopParser ()
{
	StopLexer();
	return 1;
}

ParserInfo classDeclar() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == RESWORD && !strcmp(t.lx, "class")) {
		;
	} else {
		pi.er = classExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		if (insertP(t.lx) == -1) {
			pi.er = redecIdentifier;
			pi.tk = t;
			return pi;
		}
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}
	while (t.tp != EOF && !(strcmp(t.lx, "static") && strcmp(t.lx, "field") && strcmp(t.lx, "constructor") && strcmp(t.lx, "function") && strcmp(t.lx, "method"))) {
		pi = memberDeclar();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp != EOFile) {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo memberDeclar() {
	ParserInfo pi;
	pi.er = none;

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!(strcmp(t.lx, "static") && strcmp(t.lx, "field"))) {
		pi = classVarDeclar();
		if (pi.er != none) {
			return pi;
		}
	} else if (!(strcmp(t.lx, "constructor") && strcmp(t.lx, "function") && strcmp(t.lx, "method"))) {
		pi = subroutineDeclar();
		if (pi.er != none) {
			return pi;
		}
	} else {
		pi.er = memberDeclarErr;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo classVarDeclar() {
	ParserInfo pi;
	pi.er = none;
	ClassSymbol s;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!(strcmp(t.lx, "static") && strcmp(t.lx, "field"))) {
		strcpy(s.kind, t.lx);
	} else {
		pi.er = classVarErr;
		pi.tk = t;
		return pi;
	}

	pi = type();
	if (pi.er != none) {
		return pi;
	}
	strcpy(s.type, pi.tk.lx);

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		if (insertC(t.lx, s.type, s.kind) == -1) {
			pi.er = redecIdentifier;
			pi.tk = t;
			return pi;
		}
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == ID) {
			if (insertC(t.lx, s.type, s.kind) == -1) {
				pi.er = redecIdentifier;
				pi.tk = t;
				return pi;
			}
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo type() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == RESWORD && !(strcmp(t.lx, "int") && strcmp(t.lx, "char") && strcmp(t.lx, "boolean"))) {
		pi.tk = t;
	} else if (t.tp == ID) {
		pi.tk = t;

		// Check the class in the table later
		checkClassLater(t.lx, t.ln);
	} else {
		pi.er = illegalType;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo subroutineDeclar() {
	ParserInfo pi;
	pi.er = none;
	ClassSymbol s;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!(strcmp(t.lx, "constructor") && strcmp(t.lx, "function") && strcmp(t.lx, "method"))) {
		strcpy(s.kind, t.lx);
	} else {
		pi.er = subroutineDeclarErr;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == RESWORD && !strcmp(t.lx, "void")) {
		strcpy(s.type, t.lx);
		t = GetNextToken();
	} else {
		pi = type();
		if (pi.er != none) {
			return pi;
		}
		strcpy(s.type, pi.tk.lx);
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		if (insertC(t.lx, s.type, s.kind) == -1) {
			pi.er = redecIdentifier;
			pi.tk = t;
			return pi;
		} else {
			// Emit the function name
			char str[1024];
			sprintf(str, "function %s.%s", getClassName(), t.lx);
			appendLn(str);
		}
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		;
	} else {
		pi.er = openParenExpected;
		pi.tk = t;
		return pi;
	}
	
	pi = paramList();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	pi = subroutineBody();
	if (pi.er != none) {
		return pi;
	}

	return pi;
}


ParserInfo paramList() {
	ParserInfo pi;
	pi.er = none;
	SubSymbol s;

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		return pi;
	} else if (t.tp == ID || !(strcmp(t.lx, "int") && strcmp(t.lx, "char") && strcmp(t.lx, "boolean"))) {
		pi = type();
		if (pi.er != none) {
			return pi;
		}
		strcpy(s.type, pi.tk.lx);

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == ID) {
			if (insertS(t.lx, s.type, "argument") == -1) {
				pi.er = redecIdentifier;
				pi.tk = t;
				return pi;
			}
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}

		while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
			t = GetNextToken();

			pi = type();
			if (pi.er != none) {
				return pi;
			}

			t = GetNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			} else if (t.tp == ID) {
				if (insertS(t.lx, s.type, "argument") == -1) {
					pi.er = redecIdentifier;
					pi.tk = t;
					return pi;
				}
			} else {
				pi.er = idExpected;
				pi.tk = t;
				return pi;
			}

			t = PeekNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			}
		}
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo subroutineBody() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp != EOF && strcmp(t.lx, "}")) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo statement() {
	ParserInfo pi;
	pi.er = none;

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "var")) {
		pi = varDeclarStatement();
		if (pi.er != none) {
			return pi;
		}
	} else if (!strcmp(t.lx, "let")) {
		pi = letStatement();
		if (pi.er != none) {
			return pi;
		}
	} else if (!strcmp(t.lx, "if")) {
		pi = ifStatement();
		if (pi.er != none) {
			return pi;
		}
	} else if (!strcmp(t.lx, "while")) {
		pi = whileStatement();
		if (pi.er != none) {
			return pi;
		}
	} else if (!strcmp(t.lx, "do")) {
		pi = doStatement();
		if (pi.er != none) {
			return pi;
		}
	} else if (!strcmp(t.lx, "return")) {
		pi = returnStatement();
		if (pi.er != none) {
			return pi;
		}
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo varDeclarStatement() {
	ParserInfo pi;
	pi.er = none;
	SubSymbol s;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "var")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	pi = type();
	if (pi.er != none) {
		return pi;
	}
	strcpy(s.type, pi.tk.lx);


	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		if (insertS(t.lx, s.type, "local") == -1) {
			pi.er = redecIdentifier;
			pi.tk = t;
			return pi;
		}
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == ID) {
			if (insertS(t.lx, s.type, "local") == -1) {
				pi.er = redecIdentifier;
				pi.tk = t;
				return pi;
			}
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo letStatement() {
	ParserInfo pi;
	pi.er = none;
	char output[100];
	int array = 0;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "let")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		// Check for local variable
		int index = findS(t.lx);
		char type[20];

		// Check for class variable
		if (index == -1) {
			index = findC(t.lx);

			if (index != -1) {
				strcpy(type, getKindC(index));

				if (!strcmp(type, "static")) {
					sprintf(output, "static %i", getCOffset(index));
				} else if (!strcmp(type, "field")) {
					sprintf(output, "this %i", getCOffset(index));
				}
			} else {
				pi.er = undecIdentifier;
				pi.tk = t;
				return pi;
			}
		} else {
			// It was a local variable or argument
			strcpy(type, getKindS(index));

			if (!strcmp(type, "argument")) {
				sprintf(output, "argument %i", getSOffset(index));
			} else {
				sprintf(output, "local %i", getSOffset(index));
			}
		}
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
		t = GetNextToken();
		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
			// Add push to the beginning of the output
			char newOutput[100];
			sprintf(newOutput, "push %s", output);
			appendLn(newOutput);
			appendLn("add");
			array = 1;
		} else {
			pi.er = closeBracketExpected;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "=")) {
		;
	} else {
		pi.er = equalExpected;
		pi.tk = t;
		return pi;
	}

	pi = expression();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	// Emit the assignment output after the expression is on the stack
	if (!array) {
		char newOutput[100];
		sprintf(newOutput, "pop %s", output);
		appendLn(newOutput);
	} else {
		sprintf(output, "pop temp 0");
		appendLn(output);
		sprintf(output, "pop pointer 1");
		appendLn(output);
		sprintf(output, "push temp 0");
		appendLn(output);
		sprintf(output, "pop that 0");
		appendLn(output);
	}
	return pi;
}


ParserInfo ifStatement() {
	ParserInfo pi;
	pi.er = none;
	char output[100];
	sprintf(output, "");
	int label = getLabel();

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "if")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		;
	} else {
		pi.er = openParenExpected;
		pi.tk = t;
		return pi;
	}

	pi = expression();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	// Set output to if-label
	sprintf(output, "if-goto IF_TRUE%d", label);
	appendLn(output);
	sprintf(output, "goto IF_FALSE%d", label);
	appendLn(output);

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		sprintf(output, "label IF_TRUE%d", label);
		appendLn(output);
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}
	
	while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "else")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
			sprintf(output, "label IF_FALSE%d", label);
			appendLn(output);
		} else {
			pi.er = openBraceExpected;
			pi.tk = t;
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}

		while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
			pi = statement();
			if (pi.er != none) {
				return pi;
			}

			t = PeekNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			}
		}

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
			;
		} else {
			pi.er = closeBraceExpected;
			pi.tk = t;
			return pi;
		}
	}

	// Check that output doesn't start with label IF_FALSE
	if (strncmp(output, "label IF_FALSE", 14)) {
		sprintf(output, "label IF_FALSE%d", label);
		appendLn(output);
	}
	return pi;
}


ParserInfo whileStatement() {
	ParserInfo pi;
	pi.er = none;
	char output[100];
	sprintf(output, "");
	int label = getLabel();

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "while")) {
		sprintf(output, "label WHILE_EXP%d", label);
		appendLn(output);
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		;
	} else {
		pi.er = openParenExpected;
		pi.tk = t;
		return pi;
	}

	pi = expression();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		sprintf(output, "not");
		appendLn(output);
		sprintf(output, "if-goto WHILE_END%d", label);
		appendLn(output);
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	sprintf(output, "goto WHILE_EXP%d", label);
	appendLn(output);
	sprintf(output, "label WHILE_END%d", label);
	appendLn(output);
	return pi;
}


ParserInfo doStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "do")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	pi = subroutineCall();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo subroutineCall() {
	ParserInfo pi;
	pi.er = none;
	SubCalls s;
	char output[100];
	sprintf(output, "");

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == ID) {
		// By default, assume we're calling a method of the current class
		strcpy(s.class, getClassName());
		strcpy(s.prop, t.lx);
		s.line = t.ln;
		sprintf(output, "call %s.%s", s.class, s.prop);
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	// We have an external class + method call
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == ID) {
			// The method was actually a class name or is a variable
			strcpy(s.class, s.prop);

			// Check for variable
			int index;
			if ((index = findS(s.class)) != -1) {
				strcpy(s.class, getTypeS(index));
			} else if ((index  = findC(s.class)) != -1) {
				strcpy(s.class, getTypeC(index));
			} 

			// Now we have the method name
			strcpy(s.prop, t.lx);
			s.line = t.ln;
			sprintf(output, "call %s.%s", s.class, s.prop);
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}
	}

	// Add to the array of subroutine calls to check after parsing
	checkLater(s.class, s.prop, s.line);

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		;
	} else {
		pi.er = openParenExpected;
		pi.tk = t;
		return pi;
	}

	pi = expressionList();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	// Emit the subroutine call output after the expression list is on the stack
	appendLn(output);
	return pi;
}


ParserInfo expressionList() {
	ParserInfo pi;
	pi.er = none;

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		return pi;
	}

	pi = expression();
	if (pi.er != none) {
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();

		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	return pi;
}


ParserInfo returnStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, "return")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (!strcmp(t.lx, ";")) {
		appendLn("push constant 0");
	} else if (!strcmp(t.lx, "}")) {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	} else {
		pi = expression();
		if (pi.er != none) {
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	appendLn("return");
	return pi;
}


ParserInfo expression() {
	ParserInfo pi;
	pi.er = none;

	pi = relationalExpression();
	if (pi.er != none) {
		return pi;
	}

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !(strcmp(t.lx, "&") && strcmp(t.lx, "|"))) {
		t = GetNextToken();

		if (!strcmp(t.lx, "&")) {
			appendLn("and");
		} else {
			appendLn("or");
		}

		pi = relationalExpression();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	return pi;
}


ParserInfo relationalExpression() {
	ParserInfo pi;
	pi.er = none;
	int outputLine = 0;
	char output[100];
	sprintf(output, "");

	pi = arithmeticExpression();
	if (pi.er != none) {
		return pi;
	}

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !(strcmp(t.lx, "=") && strcmp(t.lx, ">") && strcmp(t.lx, "<"))) {
		t = GetNextToken();
		outputLine = 1;

		if (!strcmp(t.lx, "=")) {
			strcpy(output, "eq");
		} else if (!strcmp(t.lx, ">")) {
			strcpy(output, "gt");
		} else {
			strcpy(output, "lt");
		}

		pi = arithmeticExpression();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	if (outputLine) {
		appendLn(output);
	}

	return pi;
}


ParserInfo arithmeticExpression() {
	ParserInfo pi;
	pi.er = none;

	pi = term();
	if (pi.er != none) {
		return pi;
	}

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !(strcmp(t.lx, "+") && strcmp(t.lx, "-"))) {
		t = GetNextToken();

		if (!strcmp(t.lx, "+")) {
			appendLn("add");
		} else {
			appendLn("sub");
		}

		pi = term();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	return pi;
}


ParserInfo term() {
	ParserInfo pi;
	pi.er = none;

	pi = factor();
	if (pi.er != none) {
		return pi;
	}

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	}

	while (t.tp == SYMBOL && !(strcmp(t.lx, "*") && strcmp(t.lx, "/"))) {
		t = GetNextToken();

		if (!strcmp(t.lx, "*")) {
			appendLn("call Math.multiply");
		} else {
			appendLn("call Math.divide");
		}

		pi = factor();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}
	}

	return pi;
}


ParserInfo factor() {
	ParserInfo pi;
	pi.er = none;
	char output[100];
	sprintf(output, "");
	int outputLine = 0;

	Token t = PeekNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == SYMBOL && !(strcmp(t.lx, "-") && strcmp(t.lx, "~"))) {
		t = GetNextToken();
		outputLine = 1;

		if (!strcmp(t.lx, "-")) {
			strcpy(output, "neg");
		} else {
			strcpy(output, "not");
		}
	}

	pi = operand();
	if (pi.er != none) {
		return pi;
	}

	if (outputLine) {
		appendLn(output);
	}
	return pi;
}


ParserInfo operand() {
	ParserInfo pi;
	pi.er = none;
	SubCalls s;
	int foundMethod = 0;
	char methodCall[100];
	sprintf(methodCall, "");
	char output[100];
	sprintf(output, "");
	int array = 0;

	Token t = GetNextToken();
	if (t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return pi;
	} else if (t.tp == INT || t.tp == STRING ) {
		if (t.tp == INT) {
			char output[100];
			sprintf(output, "push constant %s", t.lx);
			appendLn(output);
		} else {
			char output[100];
			int len = strlen(t.lx);
			sprintf(output, "push constant %i", len);
			appendLn(output);
			appendLn("call String.new");

			for (int i = 0; i < len; i++) {
				sprintf(output, "push constant %i", t.lx[i]);
				appendLn(output);
				appendLn("call String.appendChar");
			}
		}
	} else if (t.tp == RESWORD && !(strcmp(t.lx, "true") && strcmp(t.lx, "false") && strcmp(t.lx, "null") && strcmp(t.lx, "this"))) {
		if (!strcmp(t.lx, "true")) {
			appendLn("push constant 0");
			appendLn("not");
		} else if (!strcmp(t.lx, "false") || !strcmp(t.lx, "null")) {
			appendLn("push constant 0");
		} else {
			appendLn("push pointer 0");
		}
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
			;
		} else {
			pi.er = closeParenExpected;
			pi.tk = t;
			return pi;
		}
	} else if (t.tp == ID) {
		// Store the name of the ID
		Token id = t;
		
		t = PeekNextToken();
		if (t.tp == ERR) {
			pi.er = lexerErr;
			pi.tk = t;
			return pi;
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
			t = GetNextToken();

			t = GetNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			} else if (t.tp == ID) {
				strcpy(s.prop, t.lx);
				s.line = t.ln;

				// First check if its a local variable
				int index = findS(id.lx);
				if (index != -1 ) {
					// Its type is the class name
					strcpy(s.class, getTypeS(index));
				} else {
					index = findC(id.lx);
					if (index != -1) {
						// Its type is the class name
						strcpy(s.class, getTypeC(index));
					} else {
						// Assume class name
						strcpy(s.class, id.lx);
					}
				}
				// Add to the array of subroutine calls to check after parsing
				checkLater(s.class, s.prop, s.line);
				foundMethod = 1;
				sprintf(methodCall, "call %s.%s", s.class, s.prop);

				t = PeekNextToken();
				if (t.tp == ERR) {
					pi.er = lexerErr;
					pi.tk = t;
					return pi;
				}
			} else {
				pi.er = idExpected;
				pi.tk = t;
				return pi;
			}
		// The variable we stored earlier was not a class
		} else {
			// Check for local variable
			int index = findS(id.lx);

			// Check for class variable
			if (index == -1) {
				index = findC(id.lx);

				if (index == -1) {
					pi.er = undecIdentifier;
					pi.tk = id;
					return pi;
				} else {
					if (!strcmp(getKindC(index), "static")) {
						sprintf(output, "static %i", getCOffset(index));
					} else if (!strcmp(getKindC(index), "field")) {
						sprintf(output, "this %i", getCOffset(index));
					}
				}
			} else {
				if (!strcmp(getKindS(index), "argument")) {
					sprintf(output, "argument %i", getSOffset(index));
				} else {
					sprintf(output, "local %i", getSOffset(index));
				}
			}
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
			t = GetNextToken();

			pi = expression();
			if (pi.er != none) {
				return pi;
			}

			t = GetNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			} else if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
				// Add push to the beginning of the output
				char newOutput[100];
				sprintf(newOutput, "push %s", output);
				appendLn(newOutput);
				appendLn("add");
				array = 1;
			} else {
				pi.er = closeBracketExpected;
				pi.tk = t;
				return pi;
			}
		} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
			t = GetNextToken();

			pi = expressionList();
			if (pi.er != none) {
				return pi;
			}

			t = GetNextToken();
			if (t.tp == ERR) {
				pi.er = lexerErr;
				pi.tk = t;
				return pi;
			} else if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
				;
			} else {
				pi.er = closeParenExpected;
				pi.tk = t;
				return pi;
			}
		}

		if (foundMethod) {
			// Emit the subroutine call output after the expression list is on the stack
			appendLn(methodCall);
		} else if (array) {
			sprintf(output, "pop pointer 1");
			appendLn(output);
			sprintf(output, "push that 0");
			appendLn(output);
		} else {
			char newOutput[100];
			sprintf(newOutput, "push %s", output);
			appendLn(newOutput);
		}
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	return pi;
}
