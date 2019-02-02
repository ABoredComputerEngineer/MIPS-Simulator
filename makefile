$(shell mkdir -p bin )
DIRECTORY = $(shell pwd)
INCLUDE = $(DIRECTORY)/include
BIN = $(DIRECTORY)/bin
SRC = $(DIRECTORY)/src

CXX = g++


CXXFLAGS = -std=c++11 -Wall -Werror
DEBUG=-g3 -gdwarf-2

# =======================================================================
# | RULES FOR MAKING THE ASSEMBLER |
# =======================================================================
# ALL THE VARIABLES FOR MAKING THE ASSEMBLER START WITH 'ASM' PREFIX
#

ASMINCLUDE = $(INCLUDE)/assembler
ASMDEPENDENCIES = $(BIN)/main.o $(BIN)/gen.o $(BIN)/parse.o $(BIN)/lex.o $(BIN)/common.o
ASMSRC = $(SRC)/assembler
assembler: $(ASMDEPENDENCIES) 
		$(CXX) $(ASMDEPENDENCIES) $(DEBUG) -o $(BIN)/assembler 

$(BIN)/main.o: $(ASMSRC)/main.cpp $(ASMINCLUDE)/*.hpp
		$(CXX) $(CXXFLAGS) -c $(ASMSRC)/main.cpp $(DEBUG) -I $(ASMINCLUDE) -o $(BIN)/main.o
$(BIN)/gen.o: $(ASMSRC)/gen.cpp $(ASMINCLUDE)/*.hpp
		$(CXX) $(CXXFLAGS) -c $(ASMSRC)/gen.cpp $(DEBUG) -I $(ASMINCLUDE) -o $(BIN)/gen.o
$(BIN)/parse.o: $(ASMSRC)/parse.cpp $(ASMINCLUDE)/*.hpp 
		$(CXX) $(CXXFLAGS) -c $(ASMSRC)/parse.cpp $(DEBUG) -I $(ASMINCLUDE) -o $(BIN)/parse.o
$(BIN)/lex.o: $(ASMSRC)/lex.cpp $(ASMINCLUDE)/*.hpp
		$(CXX) $(CXXFLAGS) -c $(ASMSRC)/lex.cpp $(DEBUG) -I $(ASMINCLUDE) -o $(BIN)/lex.o
$(BIN)/common.o: $(ASMSRC)/common.cpp $(ASMINCLUDE)/*.hpp 
		$(CXX) $(CXXFLAGS) -c $(ASMSRC)/common.cpp $(DEBUG) -I $(ASMINCLUDE) -o $(BIN)/common.o

# =======================================================================
# | RULES FOR MAKING THE VM |
# =======================================================================
# ALL THE VARIABLES FOR MAKING THE VM START WITH 'VM' PREFIX
#

VMINCLUDE = $(INCLUDE)/VM
VMOBJS = $(BIN)/vm.o $(BIN)/printBuffer.o $(BIN)/functions.o
VMSRC = $(SRC)/VM
vm: $(VMOBJS)
	$(CXX) $(VMOBJS) $(DEBUG) -o $(BIN)/vm
$(BIN)/vm.o: $(VMSRC)/vm.cpp
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/vm.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/vm.o
$(BIN)/printBuffer.o: $(VMSRC)/printBuffer.cpp
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/printBuffer.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/printBuffer.o
$(BIN)/functions.o: $(VMSRC)/functions.cpp $(VMSRC)/printBuffer.cpp
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/functions.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/functions.o

clean:
	rm -rf $(BIN)/*
