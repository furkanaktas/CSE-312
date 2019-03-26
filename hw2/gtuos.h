#ifndef H_GTUOS
#define H_GTUOS

#define PRINT_B_CYCLE 10
#define READ_B_CYCLE 10
#define PRINT_MEM_CYCLE 10
#define READ_MEM_CYCLE 10
#define PRINT_STR_CYCLE 100
#define READ_STR_CYCLE 100
#define GET_RND_CYCLE 5


#define TCreate_CYCLE 80
#define TExit_CYCLE 50
#define TJoin_CYCLE 40
#define TYield_CYCLE 40


#define SWITCH_CYCLE 100 // context switch cycle
#define MAX_NUM_OF_THREADS 256

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <queue>
#include "8080emuCPP.h"
#include "memory.h"

using namespace std;



typedef enum {
    READY = 0,
    BLOCKED = 1,
    RUNNING = 2
} threadState;

typedef struct {
    State8080 regs;    // saved register set of 8080 chip
    int threadId;
    uint64_t startTime;      // Starting time of the thread (the cycle number of the CPU)
    uint64_t cycle;          // How many cycles the thread has used so far
    threadState state; // state of the thread (ready, blocked, running)
    uint32_t address;        // address of the memory location of the thread function
	bool isAlive;
    int blockedTime;

} threadInfo;



class GTUOS{
	public:
        GTUOS(CPU8080* cpu8080, int debug);
        uint64_t handleCall();
        void writeFile(const CPU8080 &cpu, string filename) const;
        void binarySearch(string filename)const;
        uint64_t execute();

private:
        int debugMode;
        int currRunningIndex;
        int sizeOfAliveThreads;
        uint64_t totalCycleTimes;
        uint16_t switchCycle;        //context switch cycle
        queue<threadInfo> schedular;

        CPU8080 *theCPU;
        threadInfo threadTable[MAX_NUM_OF_THREADS];
        int joinArr[MAX_NUM_OF_THREADS];


        uint64_t PRINT_B()const;
		uint64_t PRINT_MEM()const;
		uint64_t PRINT_STR()const;
		uint64_t READ_B()const;
		uint64_t READ_MEM()const;
		uint64_t READ_STR()const;
		uint64_t GET_RND()const;

		uint64_t TCreate();
        uint64_t TExit();
        uint64_t TJoin();
        uint64_t TYield();

        void contextSwitch();
        void isDebug3();
        void isDebug2(int current, int next);
};
#endif
