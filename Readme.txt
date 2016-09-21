# MipsSystemC
the soloution is defined as "Simulation of a simple MIPS Behaviour using systemC"

1.The bases of this project is on systemC library.
Please avoid to compile systemC on the Cygwin since there are some constrains not working properly on it.

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
	 this is done by adding 10 into register 0 and then adding 1 to the register [5] then we have "10" stored in register [1] and "1" then we sub register [5] from register 
	[1]  which is reg[1] -  reg[5] which is "10" - "1" then with display contition 0 all the registers will be ploted.
	on the next instruction it will check if register [0] which contains "0" on the start time is equal to register[1] which holds the counter value if not so it will 
	branch to the sub instruction again
   
ii.testMem "To test memory if works"
  It is writing in register X and then save it to memory and then this value will be loaded to register Y from the memory.
  at the end all the memory and register values will be displayed.
	this is done by adding "150" to the register [1] and then save the register[1] into address 0 of memory and then load the address 0 of memory to register[5]
	then 2 different display with value 1 and 0 will show values in memory and registers, respectively.
  
iii.testmult "To test HILO value and multiplication if works"
  is multiplig register X and Y and load it to register Z.
	this is done by adding 10 to both register [1] and [2]	and then multiplication of register [1] and [2] will go to HILO so it needed to load HILO value 
	in register [3]. at the end all register values will be ploted.

iv.mipsTest "to test pipeline on single instruction"
  it is to test pipeline working with different register addressing and all writeback works in time.
	this is done by simply adding random values to registers from [1] to [7] without any nop in middle and then plot the values of all registers.
  
6. MIPS Architecture:
  i. it is a 4 stage pipeline with {Fetch, Decode, Execute, Writeback} states. 
at the beginning one of the processes is in Fetch and the other three are in halt0,halt1 and halt2, respectively. in case of branch
detection on the Decode state the prefetched process will go to halt0 and the one in writeback will move to halt1 as the next steps,
so the loop will continue in place.
  ii. Memory instructions will be held execute state and stalls all process for 1 cycle to avoid any hazards.
  iii.structural hazards of registers will be handled on decode time and will stall all process for 2 cycle (as an assumption).
