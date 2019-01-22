# MIPS Simulator

This is a suite of programs desgined to build and run assembly wriiten for a MIPS like architecture. Currently, it consists of :

* Assembler
* Virtual Machine (VM)

See [Work In Progress](#work-in-progress) for programs that will be added in the future.

## Compiling

The source code comes with the makefile, which can be run to build the program.

To build the assembler, run:

```make assembler```

For the VM, run:

```make vm```

## Current State of the Project

### Assembler

The assembler currently supports the following instructions:

#### Arithmetic and Logical Operations

* sll
* addr
* addu
* subr
* subu
* and
* or
* xor
* nor
* andi
* ori
* xori
* addi
* addiu

#### Set instructions

* slt
* sltu
* slti
* sltiu

#### Load/Store Instructions

* lb
* lh
* lw
* lbu
* lhu
* sb
* sh
* sw

#### Branch and Jump Instructions

* beq
* jmp
* bne

The Assembler as of now only checks for syntax errors, resolves branch address and generates output files.
The binary file generated is a raw hex file with no headers ( though they will be added later on. See [TODO list](#todo-list) ) for more information.

To use the assembler :

```./assembler input_file output_path```

The input file can be any kind of text file containing the code. The arguments to the assembler *must* be in the exact order as shown above.

### Virtual Machine

To run the vm :

```./vm input_file [-d] dump_file_path```

The `-d` argument will generate a dump file in `dump_file_path`. If no path is specified, the dump file is generated in the same directory as of the input file. The generated dump file has a `.dump` extensions and is a regular text file.

By default it runs with a memory of 1024 words ( 1024 * 32 bits ) and has a text area of 256 words. Therefore it can only run program whose length is less than 256 words.

The VM as of now supports all the instructions currently supported by the Assembler ( See [Instructions Supported](#arithmetic-and-logical-operations) ). The machine currently has no input and output functionality. The only way to view the output is to generate the dump file and view it in a text editor. The input has to be hard coded in your program code.

## TODO List

### Assembler Features

* Generate a proper binary with headers in a elf-like format ( but simplified a lot )
* Add constant expression evaluation
* Add scaling to the load and save instructions so that proper byte alignment is maintained
* Add support for assembler directives
* Add more instructions
* Improve error handling

### VM Features

* Add input/output functionality
* Improve error handling

## Work in Progess

* Debugger