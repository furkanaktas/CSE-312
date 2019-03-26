#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"

	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
	if (argc != 4){
		std::cerr << "Usage: prog exeFile file.dat debugOption\n";
		exit(1); 
	}
	int DEBUG = atoi(argv[3]);

	memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS(&theCPU, DEBUG, argv[2]);


    string filename(argv[1]);
    string delimiter = ".";
    string token = filename.substr(0, filename.find(delimiter));

    theOS.binarySearch(token);


    theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);

    uint64_t cycle=0;

    cycle += theOS.execute();

    theOS.writeFile(theCPU, token);
    cout << "cyle : " << cycle << endl;
	return 0;
}

