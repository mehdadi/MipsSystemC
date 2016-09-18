#define IADDR(x)        (((x)&0x000000ff)>>2)
#define DADDR(x)        (((x)&0x000000ff)>>2)

#include "../systemc-2.3.1/include/systemc.h"
#include "dif.h"
#include "imem.h"


#define MAXSTAGE 1

SC_MODULE(mips) 
{
  sc_in<bool> CLK;
  sc_in <bool> RSTn;
  sc_uint<32>  dmem[64];      
	

	
  /*********** break SCC for pc and reg using sc_signal ***********/
  sc_signal<int> npc_sig;
  sc_signal<int> pc_sig;
  sc_signal<int> reg_sig[32];
  // Vector of bits to lock source regs in R instructions
  sc_signal<bool> reg_lock[32];
  // Counter with the currently fetched instruction, and instruction kill vector
  sc_signal<sc_uint<MAXSTAGE> > curr_inst;
  sc_bv<MAXSTAGE> inst_kill;

  
  void mips_main();
  void kill_prefetched(sc_uint<MAXSTAGE> _curr_inst);
  long long hilo;
  //int reg[32];
  int Hi;
  int Lo;
  //int pc;
	

  	SC_CTOR(mips)
	{
		SC_CTHREAD(mips_main, CLK.pos()); 
		reset_signal_is(RSTn,true);
	}
private:
    void Reset();
    unsigned int Fetch(sc_signal pc);
    DecodedStaff Decode(int ins);

      /*
 private:
           SC_HAS_PROCESS(mips);*/

}; 

