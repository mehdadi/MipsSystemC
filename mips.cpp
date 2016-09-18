#include "mips.h"

// Kill all instructions between _curr_inst (excluded) and curr_inst (included)
// properly handling overflow of the curr_inst counter.

/*void mips::kill_prefetched(sc_uint<MAXSTAGE> _curr_inst)
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
*/
/*
void DecodedStaff::printDecodedStaff()
{
    cout
            << "pc_in: " << this->pc_in
            << "pc_out: " << this->pc_out
            << "ins: " << this->ins
            << "func: " << this->func
            << "op: " << this->op
            << "value_s: " << this->value_s
            << "value_d: " << this->value_d
            << "value_t: " << this->value_t
            << "index_s: " << this->index_s
            << "index_t: " << this->index_t
            << "index_d: " << this->index_d
            << "immediate: " << this->immediate <<endl;
}
*/

void mips::Reset()
{
    for (int i = 0; i < 32; i++)
    {
        reg_sig[i].write(0);
        reg_lock[i] = (false);
    }

    inst_kill = 0;
    //curr_inst = 0;
    reg_sig[29] = 0x7fffeffc;

    pc_sig = 0x00400000;
}

DecodedStaff mips::Fetch(int pc_offset)
{
    DecodedStaff ds;
    ds.noWB = false;
    ds.pc_in = pc_sig.read();
    ds.pc_out = ds.pc_in + 4;
    ds.ins = imem[IADDR (ds.pc_in)];
    cout << "pc is: " << hex <<"0x" << ds.pc_in << "\tins is: 0x" << hex << ds.ins << "\t" << std::bitset<32>(ds.ins).to_string() << endl;
    return ds;
}

void mips::Decode(DecodedStaff &ds)
{
    ds.op  = MaskOP(ds.ins);
    cout << ds.op << endl;

    if (ds.op  == R)
    {
        ds.index_s =MaskS(ds.ins);
        ds.index_t =MaskT(ds.ins);
        ds.index_d =MaskD(ds.ins);

        // stall stage until load is done
        while (reg_lock[ds.index_s] || reg_lock[ds.index_t] || reg_lock[ds.index_d])
        {
            wait();
        }
        reg_lock[ds.index_s] = (true);
        reg_lock[ds.index_t] = (true);
        reg_lock[ds.index_d] = (true);
        ds.value_s = reg_sig[ds.index_s];
        ds.value_t = reg_sig[ds.index_t];
        ds.value_d  = reg_sig[ds.index_d];
        ds.func = MaskF(ds.ins);
    }
    else if (ds.op  == J || ds.op  == JAL)
    {
        ds.immediate = MaskJ(ds.ins);
    }
    else if (ds.ins == NOP)
    {
        return;
    }
    else
    {
        ds.index_s = MaskS(ds.ins);
        ds.index_t = MaskT(ds.ins);
        ds.immediate = MaskI(ds.ins);
        while (reg_lock[ds.index_s] || reg_lock[ds.index_t])
        {
            wait();
        }
        reg_lock[ds.index_s] = (true);
        reg_lock[ds.index_t] = (true);

        ds.value_s = reg_sig[ds.index_s];
        ds.value_t = reg_sig[ds.index_t];
    }
}

void mips::Excecute(DecodedStaff &ds)
{
    if (ds.ins == 0)
    {
        return;
    }
    else if (ds.op == R)
    {
        cout << ds.op << endl;
        switch (ds.func)
        {
            case ADDU:
                ds.value_d = ds.value_s + ds.value_t;
                break;
            case JR:
                ds.pc_out = ds.value_s;
                ds.noWB = true;
                break;
            case SUBU:
                ds.value_d = ds.value_s - ds.value_t;
                break;
            case MULTU:
                HILO = (ds.value_s) * (ds.value_t);
                break;
            case MFHI:
                ds.value_d = (HILO >> 32);
                break;
            case MFLO:
                ds.value_d = (HILO << 32);
                break;
            case AND:
                ds.value_d = ds.value_s & ds.value_t;
                break;
            case OR:
                ds.value_d = ds.value_s | ds.value_t;
                break;
            case XOR:
                ds.value_d = ds.value_s ^ ds.value_t;
                break;
                //case SRL:
                //	reg_rd = reg_rt >> shamt;
                //	break;
            case SLLV:
                ds.value_d = ds.value_t << ds.value_s;
                break;
            case SRLV:
                ds.value_d = ds.value_t >> ds.value_s;
                break;
            case SLT:
            case SLTU:
                if (ds.value_s < ds.value_t)
                    ds.value_d = 1;
                else
                    ds.value_d = 0;
                break;
            default:
                cout << "[E00] CPU Error on Func Instruction---- Rebooting \n";
                sc_stop();
                break;
        }
    }  else if (ds.op == JAL || ds.op == J)
    {
        ds.pc_out = ds.immediate << 2;
    }
    else
    {
        switch (ds.op)
        {
            case LW:
            {
                int addr = ds.value_s + ds.immediate;
                ds.value_t = dmem[DADDR (addr)];
                wait();
            }
                break;
            case SW:
            {
                int addr = ds.value_s + ds.immediate;

                dmem[DADDR (addr)] = ds.value_t;
                wait();
            }
                break;
            case BGEZ:
                ds.noWB = true;
                if (ds.value_s >= 0)
                    ds.pc_out = ds.pc_in - 4 + (ds.immediate << 2);
                break;
            case BEQ:
                ds.noWB = true;
                if (ds.value_t == ds.value_s)
                    ds.pc_out = ds.pc_in - 4 + (ds.immediate << 2);
                break;
            case BNE:
                ds.noWB = true;
                if (ds.value_t != ds.value_s)
                    ds.pc_out = ds.pc_in - 4 + (ds.immediate << 2);
                break;
            case ADDIU:
                ds.value_t = ds.value_s + ds.immediate;
                break;
            case ANDI:
                ds.value_t = ds.value_s & ds.immediate;
                break;
            case ORI:
                ds.value_t = ds.value_s | ds.immediate;
                break;
            case XORI:
                ds.value_t = ds.value_s ^ ds.immediate;
                break;
            case LUI:
                ds.value_t = ds.immediate << 16;
                break;
            case SLTIU:
            case SLTI:
                if (ds.value_s < ds.immediate)
                    ds.value_t = 1;
                else
                    ds.value_t = 0;
                break;
            default:
                cout << "[E01] CPU Error on Op Instruction---- Rebooting \n";
                sc_stop();
                break;
        }
    }
}


void mips::WriteBack(DecodedStaff &ds)
{
    if (ds.op == JAL)
    {
        reg_sig[31].write(ds.pc_in);
    }
    else if (ds.noWB)
    {

    }
    else if (ds.op == R)
    {
        reg_sig[ds.index_d].write(ds.value_d);
        reg_lock[ds.index_d] = (false);
        reg_lock[ds.index_s] = (false);
        reg_lock[ds.index_t] = (false);
    }
    else
    {
        reg_sig[ds.index_t].write(ds.value_t);
        reg_lock[ds.index_s] = (false);
        reg_lock[ds.index_t] = (false);
    }


    pc_sig.write(ds.pc_out);
}

void mips::mips_main()
{
    Reset();
    wait();
    while (true)
    {
        DecodedStaff ds = Fetch(0);
        if (ds.ins == HALT)
        {
            break;
        }
        wait();
        Decode(ds);
        wait();
        Excecute(ds);
        cout << "excec\n";
        wait();
        WriteBack(ds);
        cout << "writeback\n";
        wait();

    }
    sc_stop();
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




