#include "gtuos.h"

uint64_t GTUOS::handleCall(const CPU8080 & cpu) {

	uint64_t cycle=0;

    if (cpu.state->a == 1)
        cycle += PRINT_B(cpu);
    else if	(cpu.state->a == 2)
        cycle += PRINT_MEM(cpu);
    else if	(cpu.state->a == 3)
        cycle += READ_B(cpu);
    else if	(cpu.state->a == 4)
        cycle += READ_MEM(cpu);
    else if (cpu.state->a == 5)
        cycle += PRINT_STR(cpu);
    else if	(cpu.state->a == 6)
        cycle += READ_STR(cpu);
    else if	(cpu.state->a == 7)
        cycle += GET_RND(cpu);

    return cycle;
}

uint64_t GTUOS::PRINT_B(const CPU8080 &cpu)const{

    uint8_t b = cpu.state->b;
	cout << (int)b << endl;
	return PRINT_B_CYCLE;
}

uint64_t GTUOS::PRINT_MEM(const CPU8080 &cpu) const {

    uint32_t bc = cpu.state->b << 8 | cpu.state->c;
	cout <<(int)cpu.memory->at(bc) << endl;
	return PRINT_MEM_CYCLE;
}

uint64_t GTUOS::READ_B(const CPU8080 &cpu) const {

    cout<<"Enter number(B) : ";
    int number =0;
    cin >> number;
    while (number <0 || number > 255)
    {
        cout<< "Please enter number between 0-255 : " ;
        cin >> number;
    }
    cin.ignore();

    cpu.state->b= (uint8_t)number;
	return READ_B_CYCLE;
}

uint64_t GTUOS::READ_MEM(const CPU8080 &cpu) const {

    cout<<"Enter number(MEM) : ";
    int number =0;
    cin >> number;
    while (number <0 || number > 255)
    {
        cout<< "Please enter number between 0-255 : " ;
        cin >> number;
    }
    cin.ignore();

    uint32_t bc = cpu.state->b << 8 | cpu.state->c;
    cpu.memory->at(bc) = (uint8_t)number;
	return READ_MEM_CYCLE;
}

uint64_t GTUOS::PRINT_STR(const CPU8080 &cpu) const {

    uint32_t bc = cpu.state->b << 8 | cpu.state->c;
	uint8_t *memAddr = &(cpu.memory->at(bc));

	while ( (*memAddr) != '\0') {
		cout << *memAddr;
		++memAddr;
	}
    cout<<endl;
	return PRINT_STR_CYCLE;
}

uint64_t GTUOS::READ_STR(const CPU8080 &cpu) const {

    cout<<"Enter str : ";
    string input;
    getline(cin,input);

    uint32_t bc = cpu.state->b << 8 | cpu.state->c;
    uint8_t *memAddr = &(cpu.memory->at(bc));

    for(int i=0; i <input.length(); ++i) {
        *memAddr = input[i];
        ++memAddr;
    }
    return READ_STR_CYCLE;
}

uint64_t GTUOS::GET_RND(const CPU8080 &cpu) const {

    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    srand(t.tv_nsec);
    uint8_t random = rand()%256;

    cpu.state->b = random;
	return GET_RND_CYCLE;
}

void GTUOS::writeFile(const CPU8080 &cpu, string filename) const {

    uint8_t *memAdrr = &cpu.memory->at(0x00000);

    stringstream buffer;

    int hexz=0;
    buffer << showbase        // show the 0x prefix
           << internal        // fill between the prefix and the number
           << setfill('0');   // fill with 0s

    ofstream file;

    char temp[]  ={'.','m','e','m'};
    size_t size = sizeof(temp) + filename.length();


    char* openFile = (char*)(malloc(size));
    for (int i = 0; i <filename.length(); ++i) {
        openFile[i]=filename[i];
    }
    for (int i = filename.length(); i < size; ++i) {
        openFile[i]=temp[i-filename.length()];
    }

    file.open(openFile);
    free(openFile);
    for (int i = 0; i <4096 ; ++i) {     //i < 4096 beacuse of memory 4096*16 = 65536

        buffer << hex << setw(8) << hexz << "  ";
        file << buffer.str();
        buffer.str("");

        for (int j = 0; j <16 ; ++j) {
            buffer << hex << setw(4) << (int)*memAdrr << "  ";
            file << buffer.str();
            buffer.str("");
            ++memAdrr;

        }
        file << endl;
        buffer.str("");
        hexz +=16;
    }
    buffer.str("");
    file.close();
    return;
}

void GTUOS::binarySearch(const CPU8080 &cpu , string filename)const{
    if (filename.compare("BinarySearch") == 0)
    {
        int target=0;
        cout << "Enter target number: ";
        cin >> target;
        while (target <0 || target > 255)
        {
            cout<< "Please enter number between 0-255 : " ;
            cin >> target;
        }
        cin.ignore();

        cpu.state->b = (uint8_t)target;
    }
}