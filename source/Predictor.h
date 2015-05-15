#ifndef PREDICTOR_H
#define PREDICTOR_H

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<iomanip>
#include<sstream>
#include<stdlib.h>
#include<math.h>
#include<bitset>
struct data{
	unsigned long long address;
	//1 for Taken and 0 for Not Taken
	unsigned int branchValue;
};

struct result{
	std::string typeOfPrediction;
	unsigned long long numberOfCorrectPredictions;
};
class Predictor{
	public:
		void readFromFile(std::string fileName);
		void alwaysTaken();
		void alwaysNotTaken();
		void bimodalSingleBit(int tableSize);
		void bimodalTwoBits(int tableSize);
		void gshare(int grBits, int gshareCounterSize);
		void tournament(int bimodalTableSize, int gshareTableSize, int tournamentTableSize, int grBits);
		void extraCredit();
		void writeToFile(std::string fileName);
		void print(std::string fileName);
		
	private:
		long long numberOfBranches;
		std::vector<data> info;
		std::vector<result> output;	
};
#endif
