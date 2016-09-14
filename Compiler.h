#include <iostream>
#include <string>
#include <map>
#include <bitset>
#include <fstream>
#include <regex>

#include "dif.h"

using namespace std;

typedef bitset<8> BYTE;



struct comparer
{
    public:
    bool operator()(const std::string x, const std::string y)
    {
         return x.compare(y)<0;
    }
};

class Instruction
{
public:
	Instruction(string cmd, BYTE op, BYTE f);
	~Instruction();
	string command;
	BYTE OpCode;
	BYTE FCode;
	bool IsFunctional;
	bool hasImidiate;
	
private:

};

class Dictionary : public map<string,Instruction>
{
	public:
	Dictionary();
	~Dictionary();
	void Add(string str, BYTE op, BYTE f);
	private:
	
};

Instruction::Instruction(string cmd, BYTE op, BYTE f)
{
	command = cmd;
	OpCode = op;
	FCode = f;
}

Instruction::~Instruction()
{
} 

Dictionary::Dictionary()
{

}
Dictionary::~Dictionary()
{

}

void Dictionary::Add(string str, BYTE op, BYTE f)
{
	Instruction ins = Instruction(str,op,f);
	if (op == 0)
		ins.IsFunctional = false;
	else	
		ins.IsFunctional = true;

	ins.hasImidiate = (op == ADDIU || op == ANDI || op == BEQ || op == LUI || op == LW || op == ORI || op == SLTIU || op == SW ||op == XORI);

	this->insert(pair<string,Instruction>(ins.command,ins));
}

Dictionary myDictionary =  Dictionary();

void initMap()
{
	
	myDictionary.Add("jr",R,JR);
	myDictionary.Add("addu",R,ADDU);
	myDictionary.Add("subu",R,SUBU);
	myDictionary.Add("multu",R,MULTU);
	myDictionary.Add("mfhi",R,MFHI);
	myDictionary.Add("mflo",R,MFLO);

	myDictionary.Add("and",R,AND);
	myDictionary.Add("or",R,OR);
	myDictionary.Add("xor",R,XOR);
	myDictionary.Add("srl",R,SRL);
	myDictionary.Add("sllv",R,SLLV);
	myDictionary.Add("srlv",R,SRLV);
	myDictionary.Add("slt",R,SLT);
	myDictionary.Add("sltu",R,SLTU);
	
	myDictionary.Add("j",J,0x0);
	myDictionary.Add("jal",JAL,0x0);
	myDictionary.Add("beq",BEQ,0x0);
	myDictionary.Add("bne",BNE,0x0);
	myDictionary.Add("bgez",BGEZ,0x0);

	myDictionary.Add("addiu",ADDIU,0x0);
	myDictionary.Add("andi",ANDI,0x0);
	myDictionary.Add("ori",ORI,0x0);
	myDictionary.Add("xori",XORI,0x0);

	myDictionary.Add("lw",LW,0x0);
	myDictionary.Add("sw",SW,0x0);
	
	myDictionary.Add("lui",LUI,0x0);
	//myDictionary.Add("slti",SLTI,0x0);
	myDictionary.Add("sltiu",SLTIU,0x0);

	myDictionary.Add("Display",0,SYSCALL);
	
}
