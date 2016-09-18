#include "mips.h"

// Kill all instructions between _curr_inst (excluded) and curr_inst (included)
// properly handling overflow of the curr_inst counter.
void mips::kill_prefetched(sc_uint<MAXSTAGE> _curr_inst) 
{
UNROLL: 
	for (int i = 0; i < MAXSTAGE; i++) 
	{ 
		if (_curr_inst < curr_inst) 
		{ // No overflow
			if (i > _curr_inst && i <= curr_inst.read()) 
			{
				inst_kill[i] = true;
			}
		}
		else 
		{ // Overflow
			if (i < _curr_inst && i >= curr_inst.read())
			{
				inst_kill[i] = true;
			}
		}
	}
}

void mips::Reset()
{
    for (i = 0; i < 32; i++)
    {
        reg_sig[i] = 0;
        reg_lock[i] = false;
    }

    inst_kill = 0;
    curr_inst = 0;
    reg_sig[29] = 0x7fffeffc;

    pc_sig = 0x00400000;
}

unsigned int mips::Fetch(sc_signal pc)
{
    int pc_in = pc.read();

    int ins = imem[IADDR (_pc_in)];

    cout << "pc is: " << hex <<"0x" << _pc_in << "\tins is:" << hex << "0x" << ins << endl;

    return ins;

}

DecodedStaff mips::Decode(int ins)
{
    DecodedStaff dstaff;
    dstaff.op  = ins & MaskOP;
    if (dstaff.op  == R)
    {
        int rs;
        int rt;
        int rd;

        rd = ins & MaskD;
        rt = ins & MaskT;
        rs = ins & MaskS;

        // stall stage until load is done ASSUMING single-cycle stall
        while (reg_lock[rs] || reg_lock[rt] || reg_lock[rd])
        {
            wait();
        }

        reg_lock[rs] = true;
        reg_lock[rt] = true;
        reg_lock[rd] = true;

        dstaff.s = reg_sig[rs].read();
        dstaff.t = reg_sig[rt].read();
        dstaff.d  = reg_sig[rd].read();
        dstaff.has_i = false;
        dstaff.op = op;
        dstaff.func = ins & MaskF;
    }
    else if (dstaff.op  == J || dstaff.op  == JAL)
    {
        dstaff.i = ins & MaskJ;
        dstaff.has_i = true;
    }
    else if (ins == NOP)
    {
        return NULL; // for now we return nothing
    }
    else
    {
        
    }


    return  dstaff;
}

void mips::mips_main()
{

    Reset();
    wait();
    while (true)
    {
        int ins = Fetch();
        wait();
        Decode(ins);
        wait();
        Excecute();
        wait();
        WriteBack();
        wait();
    }

}


/*
	int i;
	int n_inst;
	int _pc_out; 
	int addr1,addr2;
	int reg_rs, reg_rt, reg_rd;
	int _pc_in, _curr_inst;
	long long HiLo;


INIT:  


	wait();


PIPE:  
	while (1) 
	{
        //main_result_out = reg_sig.read();
        // Make local copies of sc_signals for the forwarding
        _pc_in = pc_sig.read();

        // Copy and increment the instruction counter, used to kill
        _curr_inst = curr_inst.read();

        curr_inst = curr_inst.read() + 1;
        if (curr_inst.read() >= MAXSTAGE)
            curr_inst = 0;

        inst_kill[_curr_inst] = false;
        wait();
        ins = imem[IADDR (_pc_in)];

        cout << "pc is: " << hex <<"0x" << _pc_in << "\tins is:" << hex << "0x" << ins << endl;

        wait();
        _pc_out = _pc_in + 4;
		op = ins >> 26;
		switch (op)
		  {
			  case R:
				funct = ins & 0x3f;
				shamt = (ins >> 6) & 0x1f;
				rd = (ins >> 11) & 0x1f;
				rt = (ins >> 16) & 0x1f;
				rs = (ins >> 21) & 0x1f;
				  
				// stall stage until load is done ASSUMING single-cycle stall
				if (reg_lock[rs] || reg_lock[rt]) 
					wait();

				reg_rs = reg_sig[rs];
				reg_rt = reg_sig[rt];
				reg_rd = reg_sig[rd];

				switch (funct)
				{
					case ADDU:
						reg_rd = reg_rs + reg_rt;
						break;
					case JR:
						kill_prefetched(_curr_inst);
						_pc_out = reg_rs;
						break;
					case SUBU:
						reg_rd = reg_rs - reg_rt;
						break;
					case MULTU:
						HiLo = (reg_rs) * (reg_rt);
						break;
					case MFHI:
						reg_rd = (HiLo >> 32) ;
						break;
					case MFLO:
						reg_rd = (HiLo << 32);
						break; 
					case AND:
						reg_rd = reg_rs & reg_rt;
						break;
					case OR:
						reg_rd = reg_rs | reg_rt;
						break;
					case XOR:
						reg_rd = reg_rs ^ reg_rt;
						break;
					//case SRL:
					//	reg_rd = reg_rt >> shamt;
					//	break;
					case SLLV:
						reg_rd = reg_rt << reg_rs;
						break;
					case SRLV:
						reg_rd = reg_rt >> reg_rs;
						break;
					case SLT:
					case SLTU:
						if (reg_rs < reg_rt)
							reg_rd = 1;
						else
							reg_rd = 0;
						break;
					default:
						_pc_out = 0x4000000;	// error
						cout << "[E00] CPU Error on Instruction---- Rebooting \n";
						sleep(5);
						break;
				}
				break; //the end of CASE R_type:
			  
			  case J:
				  kill_prefetched(_curr_inst);
				  tgtadr = ins & 0x3ffffff;
				  _pc_out = tgtadr << 2;
				  break;
			  case JAL:
				  kill_prefetched(_curr_inst);
				  tgtadr = ins & 0x3ffffff;
				  reg_sig[31] = _pc_in;
				  _pc_out = tgtadr << 2;
				  break;
				  
			  default:
				  Imm_Add = ins & 0xffff;
				  rt = (ins >> 16) & 0x1f;
				  rs = (ins >> 21) & 0x1f;
				  // stall stage until load is done ASSUMING single-cycle stall
				  if (reg_lock[rs] || reg_lock[rt]) 
					  wait();
				  reg_rs = reg_sig[rs];
				  reg_rt = reg_sig[rt];
				  
				  switch (op)
				  {
					  case LW:
						  addr1 = reg_rs + Imm_Add;
						  reg_lock[rt] = true; // Stall next R inst
						  reg_rt = dmem[DADDR (addr1)];
						  wait();
						  reg_lock[rt] = false; // End stall next R inst
						  break;
					  case SW:
						  addr2 = reg_rs + Imm_Add;
						  // Write back only if not killed
						  if (inst_kill[_curr_inst] == false) 
						  {
							  dmem[DADDR (addr2)] = reg_rt;
						  }
						  wait();
						  break;
					  case ADDIU:
						  reg_rt = reg_rs + Imm_Add;
						  break;
					  case ANDI:
						  reg_rt = reg_rs & Imm_Add;
						  break;
					  case BGEZ:
						  kill_prefetched(_curr_inst);
						  if (reg_rs >= 0)
							  _pc_out = _pc_in - 4 + (Imm_Add << 2);
						  break;
					  case BEQ:
						  kill_prefetched(_curr_inst);
						  if (reg_rt == reg_rs)
							  _pc_out = _pc_in - 4 + (Imm_Add << 2); 
						  break;
					  case BNE:
						  kill_prefetched(_curr_inst);
						  if (reg_rt != reg_rs)
							  _pc_out = _pc_in - 4 + (Imm_Add << 2); 
						  break;
					  case ORI:
						  reg_rt = reg_rs & Imm_Add;
							  break;
					  case XORI:
						  reg_rt = reg_rs ^ Imm_Add;
						  break;
					  case LUI:
						  reg_rt = Imm_Add;
						  break;
					  case SLTIU:
					  case SLTI:
						  if (reg_rs < Imm_Add)
							  reg_rt = 1;
						  else 
							  reg_rt = 0;
						  break;
					  default:
						  _pc_out = 0x4000000;
						  cout << "[E00] CPU Error on Instruction---- Rebooting \n";
						  sleep(5);
						  break;
				  }
				  break; // the end of default
		  }
		
			// Write back only if not killed
			if (inst_kill[_curr_inst] == false)
			{
				reg_sig[0] = 0;
				reg_sig[rt].write(reg_rt);
				reg_sig[rd].write(reg_rd);
				pc_sig.write(_pc_out);
				n_inst = n_inst + 1;
			}
			wait(1);  
			HLS_EXPAND_HERE:;
	}
}

*/




