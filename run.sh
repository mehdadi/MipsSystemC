#!/bin/bash
clang++ Compiler.cpp -o comp -std=c++11
rm imem.h
./comp mipsTest >> ./imem.h
clang++ main.cpp mips.cpp -o out -I../systemc-2.3.1/include/ -L../systemc-2.3.1/lib-macosx64/ -lsystemc -lm -std=c++11
./out
