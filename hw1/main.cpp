#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"

	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
	int DEBUG = atoi(argv[2]);

	memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS;


    string filename(argv[1]);
    string delimiter = ".";
    string token = filename.substr(0, filename.find(delimiter));

    theOS.binarySearch(theCPU, token);


    theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);

    uint64_t cycle=0;
    do
	{
        if (DEBUG == 2)
        {
            string waitEnter;
            getline(cin, waitEnter);
        }
        cycle += theCPU.Emulate8080p(DEBUG);
        if(theCPU.isSystemCall()) {
            cycle += theOS.handleCall(theCPU);
        }
	}	while (!theCPU.isHalted());




    theOS.writeFile(theCPU, token);
    cout << "cyle : " << cycle << endl;
	return 0;
}

