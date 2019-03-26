#include "gtuos.h"


GTUOS::GTUOS(CPU8080* cpu8080, int debug) {

    theCPU = cpu8080;

    threadTable[0].regs = *(theCPU->state);
    threadTable[0].threadId = 0;     // Parent of os is os
    threadTable[0].startTime = 0;
    threadTable[0].cycle = 0;
    threadTable[0].state = RUNNING;
    threadTable[0].address = 0;
    threadTable[0].isAlive = true;
    threadTable[0].blockedTime = 0;


    joinArr[0] = -1;
    for (int i = 1; i < MAX_NUM_OF_THREADS ; ++i) {
        threadTable[i].isAlive = false;
        joinArr[i] = -1;
    }
    debugMode = debug;
    currRunningIndex = 0;
    sizeOfAliveThreads =1;
    totalCycleTimes = 0;
    switchCycle = 0;

    schedular.push(threadTable[0]);

}


uint64_t GTUOS::execute() {

    uint16_t tempCycle = 0;
    int tempDebug = debugMode;

    if (debugMode == 2 || debugMode == 3)
        tempDebug = 0;

    do {

        tempCycle = theCPU->Emulate8080p(tempDebug);


        if ( theCPU->isSystemCall()) {
            uint16_t temp = tempCycle;

            totalCycleTimes += tempCycle;    // TCreate  başlangıç cycle ının doğruluğu için
            switchCycle += tempCycle;

            tempCycle += this->handleCall();
            tempCycle -=temp;       // sadece handleCall dan dönen var, temp yukarda eklendi çünkü
        }

        if(theCPU->isHalted()){
            return totalCycleTimes + tempCycle;
        }

        totalCycleTimes += tempCycle;

        if ( !(theCPU->isSystemCall() && (theCPU->state->a == 10 || theCPU->state->a == 8)))
            switchCycle += tempCycle;
        else
            switchCycle=0;      // TYield ve Texit yapıldıgında, yeni thread için cycle 0 landı

        if(threadTable[currRunningIndex].state == BLOCKED || switchCycle/SWITCH_CYCLE >= 1)
        {
            contextSwitch();
            switchCycle =0;
        }


    } while (true);


    return totalCycleTimes;
}

void GTUOS::contextSwitch() {

    int nextThread;
    queue<threadInfo> tempTable;
    threadInfo temp;

    //*********************************************************************************

    if (threadTable[currRunningIndex].isAlive != true)      // TExit ten gelen switch
    {
        int tempIndex = currRunningIndex;

        if(schedular.front().state != READY)
        {
            while (!schedular.empty() && schedular.front().state != READY ) // sıradaki READY 'ye gelindi
            {
                tempTable.push(schedular.front());      // READY ye gelene kadar ki threadler çıkarıldı, temp qu nun sonuna eklenecek
                schedular.pop();
            }
            while (!tempTable.empty())
            {
                schedular.push(tempTable.front());  // eğer varsa  qu nun kalanı eklendi
                tempTable.pop();
            }
        }

        currRunningIndex = schedular.front().threadId;      // yukarda READY olan thread'e gelindi
        *theCPU->state = threadTable[currRunningIndex].regs;
        threadTable[currRunningIndex].state = RUNNING;
        threadTable[currRunningIndex].blockedTime = 0;

        for (int k = 0; k < schedular.size() ; ++k) {   // diğer thread lerin , block süreleri artırıldı
            temp = schedular.front();
            schedular.pop();
            if (currRunningIndex != temp.threadId)
            {
                threadTable[temp.threadId].blockedTime += switchCycle;
                temp.blockedTime += switchCycle;
            }
            schedular.push(temp);
        }

        isDebug2(tempIndex, currRunningIndex); //  debug 2 se print yapar
        isDebug3();     //  debug 3 se print yapar


        return;
    }

    temp = schedular.front();    // qu nun başındaki çalışan oldugu için çıkarıldı
    schedular.pop();


    while (!schedular.empty() && schedular.front().state != READY ) // sıradaki READY 'ye gelindi
    {
        tempTable.push(schedular.front());      // READY ye gelene kadar ki threadler çıkarıldı, temp qu nun sonuna eklenecek
        schedular.pop();
    }
    if (schedular.empty()){ // READY yoksa
        nextThread = temp.threadId;
    }
    else    // READY olan varsa, sıradaki thread odur
        nextThread =  schedular.front().threadId;



    if(nextThread == currRunningIndex)     // switch gerek yok, kendisiyse
    {
        if(debugMode == 2){
            cout <<"It doesn't swicth because of same process run!!"<<endl;
        }

        threadTable[currRunningIndex].cycle += switchCycle;

        schedular.push(threadTable[currRunningIndex]);  // qu nun aşına aynı thread eklendi
        while (!tempTable.empty())
        {
            schedular.push(tempTable.front());  // eğer varsa  qu nun kalanı eklendi
            tempTable.pop();
        }


        for (int k = 0; k < schedular.size() ; ++k) {   // diğer thread lerin , block süreleri artırıldı
            temp = schedular.front();
            schedular.pop();
            if (currRunningIndex != temp.threadId)
            {
                threadTable[temp.threadId].blockedTime += switchCycle;
                temp.blockedTime += switchCycle;
            }
            schedular.push(temp);
        }
        return;
    }

    else
    {

        threadTable[currRunningIndex].regs = *(theCPU->state);
        threadTable[currRunningIndex].cycle += switchCycle;

        for (int k = 0; k < schedular.size() ; ++k) {   // blocked süreleri eklendi
            temp = schedular.front();
            schedular.pop();
            if (currRunningIndex != temp.threadId)
            {
                threadTable[temp.threadId].blockedTime += switchCycle;
                temp.blockedTime += switchCycle;
            }
            schedular.push(temp);
        }

        for (int k = 0; k < tempTable.size(); ++k) { // blocked süreleri eklendi
            temp = tempTable.front();
            tempTable.pop();
            if (currRunningIndex != temp.threadId)
            {
                threadTable[temp.threadId].blockedTime += switchCycle;
                temp.blockedTime += switchCycle;
            }
            tempTable.push(temp);
        }


        isDebug2(currRunningIndex, nextThread); //  debug 2 se print yapar
        isDebug3();         //  debug 3 se print yapar




        if (threadTable[currRunningIndex].state != BLOCKED)     // block lanan , block layan tarafından açılır
            threadTable[currRunningIndex].state = READY;

        schedular.push(threadTable[currRunningIndex]);
        while (!tempTable.empty())                      // değişen thread,  schedularda güncellendi
        {
            schedular.push(tempTable.front());
            tempTable.pop();
        }

        currRunningIndex = nextThread;          // çalışacak, sıradaki thread
        *theCPU->state = threadTable[currRunningIndex].regs;
        threadTable[currRunningIndex].state = RUNNING;
        threadTable[currRunningIndex].blockedTime = 0;


        schedular.pop();
        schedular.push(threadTable[currRunningIndex]);  // çalışacak thread schedular da güncellendi
        for (int j = 0; j < schedular.size()-1; ++j) {
            temp = schedular.front();
            schedular.pop();
            schedular.push(temp);
        }

    }

}


uint64_t GTUOS::TCreate(){

    int emptyIndex = -1;

    for (int i = 0; i < MAX_NUM_OF_THREADS; ++i) {
        if (!threadTable[i].isAlive) {
            emptyIndex = i;    // tablodaki uygun yer
            break;
        }
    }

    if (emptyIndex == -1) {     // yer yoksa
        theCPU->state->b = 0;
        return TCreate_CYCLE;
    }


    uint32_t bc = theCPU->state->b << 8 | theCPU->state->c;


    bzero(&(threadTable[emptyIndex].regs), sizeof(State8080));

    threadTable[emptyIndex].regs.pc = bc;
    threadTable[emptyIndex].threadId = emptyIndex;

    threadTable[emptyIndex].regs.sp = emptyIndex*256;


    threadTable[emptyIndex].startTime = totalCycleTimes;      // bak
    threadTable[emptyIndex].cycle = 0;          // bak

    threadTable[emptyIndex].state = READY;
    threadTable[emptyIndex].address = bc;
    threadTable[emptyIndex].isAlive = true;
    threadTable[emptyIndex].blockedTime = 0;


    theCPU->state->b = emptyIndex;          // on succes,  put threadId to B register
    sizeOfAliveThreads += 1;

    schedular.push(threadTable[emptyIndex]); // schedular a, eklendi
    return TCreate_CYCLE;
}

uint64_t GTUOS::TExit() {

    if(sizeOfAliveThreads <= 1)
    {
        theCPU->state->b = 0;       // error
        return TExit_CYCLE;
    }

    threadTable[currRunningIndex].isAlive = false;

    theCPU->state->b = 1;

    for (int i = 0; i < MAX_NUM_OF_THREADS; ++i) {
        if (joinArr[i] == threadTable[currRunningIndex].threadId )  // 2 den fazla thread aynı thread'i bekleyebilir bu yüzden tüm threadlere bakıldı.
        {
            threadTable[i].state = READY;                           //  exit yapılan thread'i bekleyen , thread READY konumuna geldi
            for (int j = 0; j < schedular.size(); ++j) {
                threadInfo temp = schedular.front();
                if(temp.threadId == i)
                {
                    temp.state = READY;
                }
                schedular.pop();
                schedular.push(temp);

            }
            joinArr[i] = -1;
        }
    }
    schedular.pop();
    sizeOfAliveThreads -=1;

    contextSwitch();

    return TExit_CYCLE;
}

uint64_t GTUOS::TJoin() {

    int regB=0;
    regB = theCPU->state->b;

    if (regB == currRunningIndex)   // kendisi join ediliyorsa
    {
        perror("error !!");
        exit(-1);
    }

    if (threadTable[regB].isAlive == true )
    {
        threadTable[currRunningIndex].state = BLOCKED;
        joinArr[currRunningIndex] = regB;

        threadInfo temp = schedular.front();
        temp.state = BLOCKED;
        schedular.pop();                // join edilen thread , hem tablo'da hem schedular'da güncellendi
        schedular.push(temp);
        for (int j = 0; j < schedular.size()-1; ++j) {
            temp = schedular.front();
            schedular.pop();
            schedular.push(temp);

        }
    }

    return TJoin_CYCLE;
}

uint64_t GTUOS::TYield() {

    contextSwitch();
    return TYield_CYCLE;
}


uint64_t GTUOS::handleCall() {

	uint64_t cycle=0;

    if (theCPU->state->a == 1)
        cycle += PRINT_B();
    else if	(theCPU->state->a == 2)
        cycle += PRINT_MEM();
    else if	(theCPU->state->a == 3)
        cycle += READ_B();
    else if	(theCPU->state->a == 4)
        cycle += READ_MEM();
    else if (theCPU->state->a == 5)
        cycle += PRINT_STR();
    else if	(theCPU->state->a == 6)
        cycle += READ_STR();
    else if	(theCPU->state->a == 7)
        cycle += GET_RND();
    else if	(theCPU->state->a == 8)
        cycle += TExit();
    else if	(theCPU->state->a == 9)
        cycle += TJoin();
    else if	(theCPU->state->a == 10)
        cycle += TYield();
    else if	(theCPU->state->a == 11)
        cycle += TCreate();

    return cycle;
}


uint64_t GTUOS::PRINT_B()const{

    uint8_t b = theCPU->state->b;
	cout << (int)b << endl;
	return PRINT_B_CYCLE;
}

uint64_t GTUOS::PRINT_MEM() const {

    uint32_t bc = theCPU->state->b << 8 | theCPU->state->c;
	cout <<(int)theCPU->memory->at(bc) << endl;
	return PRINT_MEM_CYCLE;
}

uint64_t GTUOS::READ_B() const {

    cout<<"Enter number(B) : ";
    int number =0;
    cin >> number;
    while (number <0 || number > 255)
    {
        cout<< "Please enter number between 0-255 : " ;
        cin >> number;
    }
    cin.ignore();

    theCPU->state->b= (uint8_t)number;
	return READ_B_CYCLE;
}

uint64_t GTUOS::READ_MEM() const {

    cout<<"Enter number(MEM) : ";
    int number =0;
    cin >> number;
    while (number <0 || number > 255)
    {
        cout<< "Please enter number between 0-255 : " ;
        cin >> number;
    }
    cin.ignore();

    uint32_t bc = theCPU->state->b << 8 | theCPU->state->c;
    theCPU->memory->at(bc) = (uint8_t)number;
	return READ_MEM_CYCLE;
}

uint64_t GTUOS::PRINT_STR() const {

    uint32_t bc = theCPU->state->b << 8 | theCPU->state->c;
	uint8_t *memAddr = &(theCPU->memory->at(bc));

	while ( (*memAddr) != '\0') {
		cout << *memAddr;
		++memAddr;
	}
    cout<<endl;
	return PRINT_STR_CYCLE;
}

uint64_t GTUOS::READ_STR() const {

    cout<<"Enter str : ";
    string input;
    getline(cin,input);

    uint32_t bc = theCPU->state->b << 8 | theCPU->state->c;
    uint8_t *memAddr = &(theCPU->memory->at(bc));

    for(int i=0; i <input.length(); ++i) {
        *memAddr = input[i];
        ++memAddr;
    }
    return READ_STR_CYCLE;
}

uint64_t GTUOS::GET_RND() const {

    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    srand(t.tv_nsec);
    uint8_t random = rand()%256;

    theCPU->state->b = random;
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

void GTUOS::binarySearch(string filename)const{
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

        theCPU->state->b = (uint8_t)target;
    }
}

void GTUOS::isDebug3(){
    if (debugMode == 3){
        int i=0;
        int count=0;
        cout << "--------------------------------------------------------------------------------------------" << endl;
        cout <<"tid"<< setw (15);
        cout <<"startTime"<< setw (15);
        cout <<"cycle"<< setw (15);
        cout <<"state"<< setw (15);
        cout <<"address"<< setw (15);
        cout <<"blockedTime"<< setw (15);
        cout <<"           regs"<< endl;

        while (count < sizeOfAliveThreads){
            if (threadTable[i].isAlive == true){
                State8080 regs = threadTable[i].regs;    // saved register set of 8080 chip
                uint16_t threadId = threadTable[i].threadId;
                uint64_t startTime = threadTable[i].startTime;      // Starting time of the thread (the cycle number of the CPU)
                uint64_t cycle = threadTable[i].cycle;          // How many cycles the thread has used so far
                threadState state = threadTable[i].state; // state of the thread (ready, blocked, running)
                uint32_t address = threadTable[i].address;        // address of the memory location of the thread function
                int blockedTime = threadTable[i].blockedTime;

                cout <<threadId << setw (15);
                cout <<startTime << setw (15);
                cout <<cycle << setw (15);
                cout <<state << setw (15);
                cout <<address << setw (15);
                cout <<blockedTime << setw (15);
                cout <<"a: "<<(int)regs.a <<"  " <<"b: "<<(int)regs.b <<"  " <<"c: "<<(int)regs.c <<"  " <<"d: "
                     <<(int)regs.d <<"  " <<"e: "<<(int)regs.e <<"  " <<"l: "<<(int)regs.l <<"  " <<"h: "<<(int)regs.h
                     <<"  " <<"sp: "<<regs.sp <<"  " <<"pc: "<<regs.pc<< endl;
                ++count;
            }
            ++i;
        }

        cout << "--------------------------------------------------------------------------------------------" << endl;
    }
}

void GTUOS::isDebug2(int current, int next){
    if(debugMode == 2){
        int blockedTime =0;
        cout << "---------------------------------------------"<<endl;
        cout <<"Current ID: "<<current<< "    Next ID: "<<next<<endl;

        blockedTime = threadTable[next].blockedTime;
        threadTable[next].blockedTime = 0;
        cout <<"Blocked Time For Next ID: "<< blockedTime <<endl;
        cout << "---------------------------------------------"<<endl;
    }
}