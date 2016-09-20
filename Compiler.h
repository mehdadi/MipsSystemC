#include <iostream>
#include <string>
#include <map>
#include <bitset>
#include <fstream>
#include <regex>
#include <sstream>
#include <iomanip>
#include <vector>

#include "dif.h"

using namespace std;

vector<string> split(string str, char delimiter)
{
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while (getline(ss, tok, delimiter))
    {
        internal.push_back(tok);
    }

    return internal;
}

class Instruction
{
public:
	Instruction(string cmd, int op, bitset<11> f);
	~Instruction();
	string command;
	int OpCode;
	bitset<11> FCode;
	bool IsFunctional;
	bool ImJump;
private:

};

class Dictionary : public map<string,Instruction>
{
	public:
	Dictionary();
	~Dictionary();
	void Add(string str, int op, bitset<11> f);
	private:
	
};

Instruction::Instruction(string cmd, int op, bitset<11> f)
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

void Dictionary::Add(string str, int op, bitset<11> f)
{
	Instruction ins = Instruction(str,op,f);
	if (op == 0)
		ins.IsFunctional = true;
	else	
		ins.IsFunctional = false;
	ins.ImJump = op == J || op == JAL;
	this->insert(pair<string,Instruction>(ins.command,ins));
}

Dictionary myDictionary =  Dictionary();

void initMap()
{
    myDictionary.Add("nop",R,R);
	myDictionary.Add("jr",R,JR);
	myDictionary.Add("addu",R,ADDU);
	myDictionary.Add("subu",R,SUBU);
	myDictionary.Add("multu",R,MULTU);
	myDictionary.Add("mfhi",R,MFHI);
	myDictionary.Add("mflo",R,MFLO);

	myDictionary.Add("and",R,AND);
	myDictionary.Add("or",R,OR);
	myDictionary.Add("xor",R,XOR);
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
	myDictionary.Add("slti",SLTI,0x0);
	myDictionary.Add("sltiu",SLTIU,0x0);

	myDictionary.Add("display",0,SYSCALL);
	
}
