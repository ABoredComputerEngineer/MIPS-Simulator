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
VMHEADERS = $(VMINCLUDE)/*.hpp
vm: $(VMOBJS) $(BIN)/vmain.o
	$(CXX) $(VMOBJS) $(BIN)/vmain.o $(DEBUG) -o $(BIN)/vm
$(BIN)/vmain.o: $(VMSRC)/main.cpp $(VMHEADERS)
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/main.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/vmain.o
$(BIN)/vm.o: $(VMSRC)/vm.cpp $(VMHEADERS)
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/vm.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/vm.o
$(BIN)/printBuffer.o: $(VMSRC)/printBuffer.cpp
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/printBuffer.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/printBuffer.o
$(BIN)/functions.o: $(VMSRC)/functions.cpp $(VMSRC)/printBuffer.cpp $(VMHEADERS)
	$(CXX) $(CXXFLAGS) -c $(VMSRC)/functions.cpp $(DEBUG) -I $(VMINCLUDE) -o $(BIN)/functions.o

# =======================================================================
# | RULES FOR MAKING THE VM |
# =======================================================================
# ALL THE VARIABLES FOR MAKING THE DEBUGGER ( Backend ) START WITH 'DB' PREFIX
# The vm is also a dependency of the debugger 
#
#

DBINCLUDE = $(INCLUDE)/Debugger
DBOBJS = $(BIN)/dbg.o $(BIN)/linenoise.o $(BIN)/debugMachine.o $(BIN)/dbmain.o
DBSRC = $(SRC)/Debugger
DBHEADERS = $(DBINCLUDE)/*.hpp $(VMHEADERS) $(DBINCLUDE)/linenoise.h

debugger: $(DBOBJS) $(VMOBJS)
	$(CXX) $(DBOBJS) $(VMOBJS) $(DEBUG) -o $(BIN)/debug

$(BIN)/dbg.o : $(DBSRC)/debug.cpp $(DBHEADERS)
	$(CXX) $(CXXFLAGS) -c $(DBSRC)/debug.cpp $(DEBUG) -I $(INCLUDE) -I $(DBINCLUDE) -o $(BIN)/dbg.o

$(BIN)/dbmain.o: $(DBSRC)/main.cpp $(DBHEADERS)
	$(CXX) $(CXXFLAGS) -c $(DBSRC)/main.cpp $(DEBUG) -I $(INCLUDE) -I $(DBINCLUDE) -o $(BIN)/dbmain.o

$(BIN)/debugMachine.o: $(DBSRC)/debugMachine.cpp $(DBHEADERS)
	$(CXX) $(CXXFLAGS) -c $(DBSRC)/debugMachine.cpp $(DEBUG) -I $(INCLUDE) -I $(DBINCLUDE) -o $(BIN)/debugMachine.o

$(BIN)/linenoise.o: $(DBSRC)/linenoise.c $(DBINCLUDE)/linenoise.h
	gcc -c $(DBSRC)/linenoise.c -I $(INCLUDE) -I $(DBINCLUDE) -o $(BIN)/linenoise.o


clean:
	rm -rf $(BIN)/*
