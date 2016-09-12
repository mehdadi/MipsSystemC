#include "Compiler.h"

int main(int argc, char** argv)
{
	if (argc < 2) 
    {
        cout << "wrong usage...\n";
        return 0;
    }
	initMap();
	cout <<"map genrated \n";
    string path  = argv[0];
    ifstream ifs;
    ofstream ofs;
    ifs.open(argv[0], ifstream::in);
    ofs.open(argv[1], ifstream::out);
    
    string line;
    while(getline(ifs,line))
    {
        cout << line << endl;
        line.split(' ');
        

        if (ifs.eof())
        {
            break;
        }
    }



	return 0;
}


/*



    string binary_str("11001111");
    bitset<8> set(binary_str);  
    cout << hex << set.to_ulong() << endl;
*/