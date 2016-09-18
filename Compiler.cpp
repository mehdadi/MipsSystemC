#include "Compiler.h"

//#define bitset<32> WORD;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "wrong usage...\n";
        return 0;
    }
    initMap();

    long count = 0x00400000;

    ifstream ifs(argv[1]);
    ofstream ofs(argv[2]);

    ofs << "const unsigned long imem[] = {" <<endl;


    string line;
    while (getline(ifs, line))
    {
        if (ifs.eof())
        {
            break;
        }

        vector<string> comm = split(line, ' ');
        string first = comm[0];
        string second = comm[1];

        map<string, Instruction>::iterator val = myDictionary.find(first);
        if (val == myDictionary.end())
        {
            cout << "error in file" << endl;
            break;
        }

        string spart = "";
        spart += val->second.OpCode.to_string();
        if (val->second.ImJump)
        {
             spart += bitset<26>(stoi(second)).to_string();
            //spart << hex << std::setw(6) << std::setfill('0') << stoi(second);
        }
        else
        {
            vector<string> splited = split(second,',');
            if (val->second.IsFunctional)
            {
                spart += bitset<5>(stoi((splited[0]))).to_string();
                spart += bitset<5>(stoi((splited[1]))).to_string();
                spart += bitset<5>(stoi((splited[2]))).to_string();
                spart += bitset<11>(val->second.FCode.to_string()).to_string();
            } else
            {
                spart += bitset<5>(stoi(splited[0])).to_string();
                spart += bitset<5>(stoi(splited[1])).to_string();
                spart += bitset<16>(stoi(splited[2])).to_string();
            }
        }

        ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << bitset<32>(spart).to_ulong()
            << ", //\t [0x" << hex  << std::setw(8) << std::setfill('0') << count << "]\t" << line <<  endl;
         count += 4;
    }

    ofs << "};" <<endl;

    ifs.close();
    ofs.close();

    return 0;
}


/*
    string binary_str("11001111");
    bitset<8> set(binary_str);  
    cout << hex << set.to_ulong() << endl;
*/
