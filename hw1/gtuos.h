#ifndef H_GTUOS
#define H_GTUOS

#define PRINT_B_CYCLE 10
#define READ_B_CYCLE 10
#define PRINT_MEM_CYCLE 10
#define READ_MEM_CYCLE 10
#define PRINT_STR_CYCLE 100
#define READ_STR_CYCLE 100
#define GET_RND_CYCLE 5


#include <string>
#include "8080emuCPP.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>


using namespace std;

class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
        void writeFile(const CPU8080 &cpu, string filename) const;
        void binarySearch(const CPU8080 &cpu , string filename)const;
	private:
		uint64_t PRINT_B(const CPU8080 &cpu)const;
		uint64_t PRINT_MEM(const CPU8080 &cpu)const;
		uint64_t PRINT_STR(const CPU8080 &cpu)const;
		uint64_t READ_B(const CPU8080 &cpu)const;
		uint64_t READ_MEM(const CPU8080 &cpu)const;
		uint64_t READ_STR(const CPU8080 &cpu)const;
		uint64_t GET_RND(const CPU8080 &cpu)const;
};

#endif
