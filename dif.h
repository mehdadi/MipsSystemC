#define R 0x0  //op

#define JR 0x8  //func		i

#define ADDU 0x21  //func	i
#define SUBU 0x23  //func	i

//#define NOP 0x0 
//#define MULT 0x18  //func 
#define MULTU 0x19 //func	i

#define MFHI 0x10  //func	i
#define MFLO 0x12  //func	i

#define AND 0x24  //func	i
#define OR 0x25 //func		i
#define XOR 0x26 //func		i
#define SRL 0x2 //func		i
#define SLLV 0x4 //func		i
#define SRLV 0x6 //func		i

#define SLT 0x2A //func		i
#define SLTU 0x2B //func	i

#define J 0x2 //op			i
#define JAL 0x3 //op		i

#define BEQ 0x4 //op		i
#define BNE 0x5 //op		i
#define BGEZ 0x1 //op		i

#define ADDIU 0x9 //op		i
#define ANDI 0x0C //op		i
#define ORI 0x0D //op		i
#define XORI 0x0E //op		i

#define LW 0x23 //op		i
#define SW 0x2B //op		i

#define LUI 0xF //op		i
#define SLTI 0xA //op		i
#define SLTIU 0xB //op		i

#define SYSCALL 0x0C //Func
//Utils

