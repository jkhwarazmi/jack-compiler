CC     ?= cc
CFLAGS ?= -O2

SRC  := src/lexer.c src/parser.c src/symbols.c src/compiler.c
HDR  := $(wildcard src/*.h)
BIN  := jackc
ROOT := ../..

.PHONY: all test test-lexer test-symbols test-codegen clean

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -Isrc -o $@ $(SRC)

test: test-lexer test-symbols test-codegen

test-lexer:
	cd tests/lexer && $(CC) $(CFLAGS) -DTEST -I$(ROOT)/src -o LexerGrader LexerGrader.c $(ROOT)/src/lexer.c && ./LexerGrader

test-symbols:
	cd tests/symbols && $(CC) $(CFLAGS) -DTEST_COMPILER -I$(ROOT)/src -o SymbolGrader SymbolGrader.c $(addprefix $(ROOT)/,$(SRC)) && ./SymbolGrader

test-codegen:
	cd tests/codegen && $(CC) $(CFLAGS) -DTEST_COMPILER -I$(ROOT)/src -o CodeGrader CodeGrader.c $(addprefix $(ROOT)/,$(SRC)) && ./CodeGrader

clean:
	rm -f $(BIN) tests/*/*Grader tests/codegen/SourceFiles.txt
	find tests -name '*.vm' -not -path '*_compiled/*' -delete
