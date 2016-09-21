# MipsSystemC
This Project is based on a course compilation request by Professor Luciano Lavagno

1.The bases of this project is on systemC library and c++ compiled using clang++.
Please avoid to compile systemC on the Cygwin since there is some constrains not working properly on it.

2. Contains two projects:
  i.Compiler in which translate the simple MIPS assembly code to the machine code.
  ii.Main which is the emulator of the MIPS base on systemC library.

3.Compiling:
using std as c++11 is mandatory for both projects.
the file dif.h is a shared file between Compiler and Main.

 i. Compiling the Compiler.cpp
  CXX is defined as your standard c++ compiler on Macosx is defined Clang++ and on Linux Ubuntu Kernel it is g++.
  > CXX Compiler.cpp -o comp -std=c++11
  
ii.Compiling the Main.cpp
  > CXX main.cpp mips.cpp -o out -I./systemc-2.3.1/include/ -L./systemc-2.3.1/lib-SUFFIX/ -lsystemc -lm

the include and library is depended on the instalation version and place of compiled systemC.

4.Run:
(0) "your testfile"
>rm imem.h
>./comp (0) >> imem.h
>sh make.sh
>./out

5.Test Files
  Due to current pipeline architecture of implemented MIPS it is needed to avoid structural hazards by inserting nop on read after write of registers.

i.testj "To test branching if works"
  it is counting from 10 to 0 in a loop and halt the system.
ii.testMem "To test memory if works"
  It is writing in register X and then save it to memory and then this value will be loaded to register Y from the memory.
  at the end all the memory and register values will be displayed.
iii.testmult "To test HILO value and multiplication if works"
  is multiplig register X and Y and load it to register Z.

6. MIPS Architecture:
  i. it is a 4 stage pipeline with {Fetch, Decode, Execute, Writeback} states. 
at the beginning one of the processes is in Fetch and the other three are in halt0,halt1 and halt2, respectively. in case of branch
detection on the Decode state the prefetched process will go to halt0 and the one in writeback will move to halt1 as the next steps,
so the loop will continue in place.
  ii. Memory instructions will be held execute state and stalls all process for 1 cycle to avoid any hazards.
  iii.structural hazards of registers will be handled on decode time and will stall all process for 2 cycle (as an assumption).
