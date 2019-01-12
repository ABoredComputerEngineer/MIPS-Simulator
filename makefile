$(shell mkdir -p bin )
DIRECTORY = $(shell pwd)
INCLUDE = $(DIRECTORY)/include
BIN = $(DIRECTORY)/bin
SRC = $(DIRECTORY)/src

CXX = g++


CXXFLAGS = -std=c++11 -Wall -Werror
DEBUG_FLAG = -g3 -gdwarf-2

DEPENDENCIES = $(BIN)/main.o $(BIN)/gen.o $(BIN)/parse.o $(BIN)/lex.o $(BIN)/common.o

COMMON_HEADER = $(INCLUDE)/common.hpp
LEX_HEADER = $(COMMON_HEADER) $(INCLUDE)/lex.hpp
PARSE_HEADER = $(LEX_HEADER) $(INCLUDE)/parse.hpp
GEN_HEADER = $(PARSE_HEADER) $(INCLUDE)/gen.hpp
MAIN_HEADER = $(GEN_HEADER)
assembler: $(DEPENDENCIES) 
		$(CXX) -o $(BIN)/assembler $(DEPENDENCIES) $(DEBUG+FLAG)

$(BIN)/main.o: $(SRC)/main.cpp $(MAIN_HEADER)
		$(CXX) $(CXXFLAGS) -c $(SRC)/main.cpp $(DEBUG_FLAG) -o $(BIN)/main.o
$(BIN)/gen.o: $(SRC)/gen.cpp $(GEN_HEADER)
		$(CXX) $(CXXFLAGS) -c $(SRC)/gen.cpp $(DEBUG_FLAG) -o $(BIN)/gen.o
$(BIN)/parse.o: $(SRC)/parse.cpp $(PARSE_HEADER)
		$(CXX) $(CXXFLAGS) -c $(SRC)/parse.cpp $(DEBUG_FLAG) -o $(BIN)/parse.o
$(BIN)/lex.o: $(SRC)/lex.cpp $(LEX_HEADER)
		$(CXX) $(CXXFLAGS) -c $(SRC)/lex.cpp $(DEBUG_FLAG) -o $(BIN)/lex.o
$(BIN)/common.o: $(SRC)/common.cpp $(COMMON_HEADER)
		$(CXX) $(CXXFLAGS) -c $(SRC)/common.cpp $(DEBUG_FLAG) -o $(BIN)/common.o
