#include "Compiler.h"

#define bitset<32> WORD;

int main(int argc, char** argv)
{
	if (argc < 2) 
    {
        cout << "wrong usage...\n";
        return 0;
    }
	initMap();
	cout <<"map genrated \n";

    ifstream ifs(argv[1]);
    ofstream ofs(argv[2]);
    
    string line;
    while(getline(ifs,line))
    {

        int pos = line.find(' ',0);
        string comm = line.substr(0,pos);
        
        map<string,Instruction>::iterator val = myDictionary.find(comm);
        if (val == myDictionary.end())
        {
            cout <<"error in file"<<endl;
            break;
        }
        
        WORD result;
        result 
        cout << line << "\t0x" << result.to_string() << endl;
        
        if (ifs.eof())
        {
            break;
        }
    }

    ifs.close();

	return 0;
}


/*



    string binary_str("11001111");
    bitset<8> set(binary_str);  
    cout << hex << set.to_ulong() << endl;
*/