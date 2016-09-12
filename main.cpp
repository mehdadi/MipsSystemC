#include "mips.h"


int sc_main(int argc, char *argv[])
{	
	sc_clock  CLK;
	sc_signal<bool> RSTn;

    mips mips("mips");
	mips.CLK(CLK);
	mips.RSTn(RSTn);  
	
	//mips.main_result_out(result);	
	
	sc_start();
	
    return 0;
}