#include "mips.h"

void mips::printDecodedStaff(DecodedStaff &ds)
{
    cout
            << " pc_in: " << ds.pc_in
            << " ins: " << ds.ins
            << " func: " << ds.func
            << " op: " << ds.op
            << " value_s: " << ds.value_s
            << " value_d: " << ds.value_d
            << " value_t: " << ds.value_t
            << " index_s: " << ds.index_s
            << " index_t: " << ds.index_t
            << " index_d: " << ds.index_d
            << " immediate: " << ds.immediate << endl;
}


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

DecodedStaff mips::Fetch(sc_signal<int> &pc)
{
    DecodedStaff ds;
    ds.noWB = false;
    ds.pc_in = pc_sig.read();
    //npc_sig.write(ds.pc_in + 4);
    pc_sig.write(ds.pc_in + 4);
    ds.ins = imem[IADDR (ds.pc_in)];
    cout << "pc is: " << hex <<"0x" << ds.pc_in << "\tins is: 0x" << hex << ds.ins << "\t" << std::bitset<32>(ds.ins).to_string() << endl;
    return ds;
}

bool mips::Decode(DecodedStaff &ds)
{
    ds.op  = MaskOP(ds.ins);
    if (ds.ins == NOP)
    {
        return false;
    }
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

        return false;
    }
    else if (ds.op  == J || ds.op  == JAL)
    {
        ds.immediate = MaskJ(ds.ins);
        ds.noWB = true;
        return true;
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
        return ds.op == BEQ || ds.op == BGEZ || ds.op == BNE;
    }
}

void mips::Excecute(DecodedStaff &ds)
{
    if (ds.ins == 0)
    {
        return;
    }
    else if (ds.ins == HALT)
    {
        cout << "System Halted" << endl;
        return;
    }
    else if (ds.op == R)
    {
        switch (ds.func)
        {
            case ADDU:
                ds.value_d = ds.value_s + ds.value_t;
                break;
            case JR:
                pc_sig.write(ds.value_s);
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
            case SYSCALL:
                for (int ind = 0; ind < 32; ind++)
                {
                    cout << "reg(" << ind << ") = " << reg_sig[ind] << endl;
                }
                break;
            default:
                cout << "[E00] CPU Error on Func Instruction---- Rebooting \n";
                sc_stop();
                break;
        }
    }  else if (ds.op == JAL || ds.op == J)
    {
        pc_sig.write(ds.immediate << 2);
    }
    else
    {
        switch (ds.op)
        {
            cout << ds.op << endl;
            case LW:
            {
                int addr = ds.value_s + ds.immediate;
                ds.value_t = dmem[DADDR (addr)];
                wait(); //hazard on pipeline
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
                    pc_sig.write(ds.pc_in - 4 + (ds.immediate << 2));
                break;
            case BEQ:
                ds.noWB = true;
                if (ds.value_t == ds.value_s)
                    pc_sig.write(ds.pc_in - 4 + (ds.immediate << 2));
                break;
            case BNE:
                ds.noWB = true;
                if (ds.value_t != ds.value_s)
                    pc_sig.write(ds.pc_in - 4 + (ds.immediate << 2));
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
    if (ds.ins == NOP)
    {
        return;
    }
    else if (ds.op == JAL)
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
}

void mips::mips_main()
{
    Reset();
    wait();

    DecodedStaff ds[4];
    exStatus current_states[4] { fetch, halt0, halt1, halt2 };

    while (true)
    {
        for (int i = 0 ; i < 1 ; i++)
        {
            bool branch;
            switch (current_states[i])
            {
                case fetch:
                {
                    ds[i] = Fetch(pc_sig);
                }break;
                case decode:
                {
                    branch = Decode(ds[i]);
                }break;
                case exceute:
                {
                    Excecute(ds[i]);
                }break;
                case writeback:
                {
                    WriteBack(ds[i]);
                }break;
            }

            if (branch == false)
            {
                current_states[i] = getNextStatus(current_states[i]);
            }
            else
            {
                for (int b = 0 ; b < 4 ; b++)
                {
                    if (b != i)
                    {
                        if (current_states[b] == fetch)
                        {
                            // there will be one for sure in WB so this wont go over
                            current_states[b] = halt0;
                        }
                        else
                        {
                            current_states[b] = getNextStatus(current_states[b]);
                        }
                    }
                }
            }
        }

        if ( (ds[0].ins == HALT && current_states[0] == writeback)||
                (ds[1].ins == HALT && current_states[1] == writeback)||
                (ds[2].ins == HALT && current_states[2] == writeback)||
                (ds[3].ins == HALT && current_states[3] == writeback))
        {
            break;
        }
        wait();

    }

    sc_stop();
}


/*
ds0 = Fetch(0);
wait();
ds1 = Fetch(1);
Decode(ds0);
wait();
ds2 = Fetch(2);
Decode(ds1);
Excecute(ds0);
wait();
ds3 = Fetch(3);
Decode(ds2);
Excecute(ds1);
WriteBack(ds0);
wait();
*/


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




