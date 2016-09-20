#define R 0x0  //op
#define HALT 0xFFFFFFFF


#define JR 0x8  //func		i           nPC = s

#define ADDU 0x21  //func	i           d = s + t
#define SUBU 0x23  //func	i           d = s - t

#define NOP 0x0
//#define MULT 0x18  //func 
#define MULTU 0x19 //func	i           LO = s * t

#define MFHI 0x10  //func	i           d = HI
#define MFLO 0x12  //func	i           d = LO

#define AND 0x24  //func	i           d = s & t
#define OR 0x25 //func		i           d = s | t
#define XOR 0x26 //func		i           d = s ^ t
//#define SRL 0x2 //func		i
#define SLLV 0x4 //func		i   std     d = t << s
#define SRLV 0x6 //func		i   std     d = t >> s

#define SLT 0x2A //func		i           set(d)  if s<t else reset (d)
#define SLTU 0x2B //func	i           set(d)  if s<t else reset(d)

#define J 0x2 //op			i   iiiii   go to pc(i)
#define JAL 0x3 //op		i   iiiii   go to pc(i) save return into r(31)

#define BEQ 0x4 //op		i   sti     go to pc(i) if s == t
#define BNE 0x5 //op		i   sti     go to pc(i) if s != t
#define BGEZ 0x1 //op		i   sti     go to pc(i) if s >= 0, force(t,0)

#define ADDIU 0x9 //op		i   sti     t = s + i
#define ANDI 0x0C //op		i   sti     t = s & i
#define ORI 0x0D //op		i   sti     t = s | i
#define XORI 0x0E //op		i   sti     t = s ^ i

#define LW 0x23 //op		i   sti     t= MEM[s+i]
#define SW 0x2B //op		i   sti     MEM[s+i] = t

#define LUI 0xF //op		i   -ti     t = i << 16
#define SLTI 0xA //op		i
#define SLTIU 0xB //op		i   sti     set(t) if s<i else reset(t)

#define SYSCALL 0x0C //Func
//Utils

//Masks
#define MaskOP(x) (((x) >> 26) & 0x3F)
#define MaskF(x) ((x)&0x0000003F)
#define MaskS(x) (((x)>>21) & 0x1F)
#define MaskT(x) (((x)>>16) & 0x1F)
#define MaskD(x) (((x)>>11) & 0x1F)
#define MaskI(x) ((x)&0x0000FFFF)
#define MaskJ(x) ((x)&0x03FFFFFF)

#define PCStartAddr 0x40000

enum exStatus : int
{
    fetch = 0,
    decode = 1,
    exceute = 2,
    writeback = 3,
    halt0 = -1,
    halt1 = -2,
    halt2 = -3,
};



struct DecodedStaff
{
    bool noWB;
    int pc_in;
    int ins;
    int func;
    int op;
    int value_s;
    int value_d;
    int value_t;
    int index_s;
    int index_t;
    int index_d;
    int immediate;
};



