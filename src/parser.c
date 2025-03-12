#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


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
		default:
			printf("Line %i at or near %s: syntax error\n", pi.tk.ln, pi.tk.lx);
			break;
	}

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
	if (t.tp == RESWORD && !strcmp(t.lx, "class")) {
		;
	} else {
		pi.er = classExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (t.tp != EOF && !(strcmp(t.lx, "static") && strcmp(t.lx, "field") && strcmp(t.lx, "constructor") && strcmp(t.lx, "function") && strcmp(t.lx, "method"))) {
		pi = memberDeclar();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
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
	if (!(strcmp(t.lx, "static") && strcmp(t.lx, "field"))) {
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

	Token t = GetNextToken();
	if (!(strcmp(t.lx, "static") && strcmp(t.lx, "field"))) {
		;
	} else {
		pi.er = classVarErr;
		pi.tk = t;
		return pi;
	}

	pi = type();
	if (pi.er != none) {
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();
		t = GetNextToken();
		
		if (t.tp == ID) {
			;
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
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
	if (t.tp == RESWORD && !(strcmp(t.lx, "int") && strcmp(t.lx, "char") && strcmp(t.lx, "boolean"))) {
		;
	} else if (t.tp == ID) {
		;
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

	Token t = GetNextToken();
	if (!(strcmp(t.lx, "constructor") && strcmp(t.lx, "function") && strcmp(t.lx, "method"))) {
		;
	} else {
		pi.er = subroutineDeclarErr;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "void")) {
		t = GetNextToken();
	} else {
		pi = type();
		if (pi.er != none) {
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
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

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		return pi;
	} else if (t.tp == ID || !(strcmp(t.lx, "int") && strcmp(t.lx, "char") && strcmp(t.lx, "boolean"))) {
		pi = type();
		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (t.tp == ID) {
			;
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}

		t = PeekNextToken();
		while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
			t = GetNextToken();

			pi = type();
			if (pi.er != none) {
				return pi;
			}

			t = GetNextToken();
			if (t.tp == ID) {
				;
			} else {
				pi.er = idExpected;
				pi.tk = t;
				return pi;
			}

			t = PeekNextToken();
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
	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (t.tp != EOF && strcmp(t.lx, "}")) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
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
	if (!strcmp(t.lx, "var")) {
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

	Token t = GetNextToken();
	if (!strcmp(t.lx, "var")) {
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

	t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();
		t = GetNextToken();
		
		if (t.tp == ID) {
			;
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
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

	Token t = GetNextToken();
	if (!strcmp(t.lx, "let")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
		t = GetNextToken();
		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
			;
		} else {
			pi.er = closeBracketExpected;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "=")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo ifStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (!strcmp(t.lx, "if")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (!strcmp(t.lx, "else")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
			;
		} else {
			pi.er = openBraceExpected;
			pi.tk = t;
			return pi;
		}

		t = PeekNextToken();
		while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
			pi = statement();
			if (pi.er != none) {
				return pi;
			}
			t = PeekNextToken();
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
			;
		} else {
			pi.er = closeBraceExpected;
			pi.tk = t;
			return pi;
		}
	}

	return pi;
}


ParserInfo whileStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (!strcmp(t.lx, "while")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		;
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	while (!(strcmp(t.lx, "var") && strcmp(t.lx, "let") && strcmp(t.lx, "if") && strcmp(t.lx, "while") && strcmp(t.lx, "do") && strcmp(t.lx, "return"))) {
		pi = statement();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		;
	} else {
		pi.er = closeBraceExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo doStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (!strcmp(t.lx, "do")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
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

	Token t = GetNextToken();
	if (t.tp == ID) {
		;
	} else {
		pi.er = idExpected;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ID) {
			;
		} else {
			pi.er = idExpected;
			pi.tk = t;
			return pi;
		}
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		;
	} else {
		pi.er = closeParenExpected;
		pi.tk = t;
		return pi;
	}

	return pi;
}


ParserInfo expressionList() {
	ParserInfo pi;
	pi.er = none;

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		return pi;
	}

	pi = expression();
	if (pi.er != none) {
		return pi;
	}

	t = PeekNextToken();
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		t = GetNextToken();

		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = PeekNextToken();
	}

	return pi;
}


ParserInfo returnStatement() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (!strcmp(t.lx, "return")) {
		;
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	t = PeekNextToken();
	if (!strcmp(t.lx, ";")) {
		;
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
	if (t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		;
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
		return pi;
	}

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
	while (t.tp == SYMBOL && !(strcmp(t.lx, "&") && strcmp(t.lx, "|"))) {
		t = GetNextToken();

		pi = relationalExpression();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	return pi;
}


ParserInfo relationalExpression() {
	ParserInfo pi;
	pi.er = none;

	pi = arithmeticExpression();
	if (pi.er != none) {
		return pi;
	}

	Token t = PeekNextToken();
	while (t.tp == SYMBOL && !(strcmp(t.lx, "=") && strcmp(t.lx, ">") && strcmp(t.lx, "<"))) {
		t = GetNextToken();

		pi = arithmeticExpression();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
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
	while (t.tp == SYMBOL && !(strcmp(t.lx, "+") && strcmp(t.lx, "-"))) {
		t = GetNextToken();

		pi = term();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
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
	while (t.tp == SYMBOL && !(strcmp(t.lx, "*") && strcmp(t.lx, "/"))) {
		t = GetNextToken();

		pi = factor();
		if (pi.er != none) {
			return pi;
		}
		t = PeekNextToken();
	}

	return pi;
}


ParserInfo factor() {
	ParserInfo pi;
	pi.er = none;

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && !(strcmp(t.lx, "-") && strcmp(t.lx, "~"))) {
		t = GetNextToken();
	}

	pi = operand();
	if (pi.er != none) {
		return pi;
	}

	return pi;
}


ParserInfo operand() {
	ParserInfo pi;
	pi.er = none;

	Token t = GetNextToken();
	if (t.tp == INT || t.tp == STRING ) {
		;
	} else if (t.tp == RESWORD && !(strcmp(t.lx, "true") && strcmp(t.lx, "false") && strcmp(t.lx, "null") && strcmp(t.lx, "this"))) {
		;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		pi = expression();
		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
			;
		} else {
			pi.er = closeParenExpected;
			pi.tk = t;
			return pi;
		}
	} else if (t.tp == ID) {
		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
			t = GetNextToken();

			t = GetNextToken();
			if (t.tp == ID) {
				t = PeekNextToken();
			} else {
				pi.er = idExpected;
				pi.tk = t;
				return pi;
			}
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
			t = GetNextToken();

			pi = expression();
			if (pi.er != none) {
				return pi;
			}

			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
				;
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
			if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
				;
			} else {
				pi.er = closeParenExpected;
				pi.tk = t;
				return pi;
			}
		}
	} else {
		pi.er = syntaxError;
		pi.tk = t;
		return pi;
	}

	return pi;
}


#ifndef TEST_PARSER
int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	if (!InitParser(argv[1])) {
		printf("Error opening file\n");
		return 1;
	}

	ParserInfo pi = Parse();
	if (pi.er == none) {
		printf("No syntax errors found\n");
	}
	StopParser();
	return 1;
}
#endif
