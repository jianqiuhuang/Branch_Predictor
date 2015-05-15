#include "Predictor.h"

//counter sizes and global register size
static const int grBits = 6, gshareCounterSize = 2048, bimodalCounterSize = 2048, tournamentCounterSize = 2048, numberOfCounters = 7;;
int main(int argc, char **argv){
	if(argc != 3){
		std::cerr << "Usage: ./predictor input_file.txt output_file.txt" << std::endl;
		exit(1);
	}

	//Bimodal predictor counter size
	int counterSize[] = {16, 32, 128, 256, 512, 1024, 2048};
	
	Predictor predict;
	//Process data from input file to data vector
	predict.readFromFile(argv[1]);
	//Perform Always Taken Prediction
	predict.alwaysTaken();
	//Perform Always Not Taken Prediction
	predict.alwaysNotTaken();
	//Perform bimodal Single Bit Prediction with different counter size
	for(int i = 0; i < numberOfCounters; i++){
		predict.bimodalSingleBit(counterSize[i]);
	}
	//Perform bimodal Two Bit Prediction with different counter size
	for(int i = 0; i < numberOfCounters; i++){
		predict.bimodalTwoBits(counterSize[i]);
	}
	//Perform gshare Prediction with different global bit size
	for(int i = 3; i <= grBits; i++){
		predict.gshare(i, gshareCounterSize);
	}
	//Perfor tournament Prediction
	predict.tournament(bimodalCounterSize, gshareCounterSize, tournamentCounterSize, grBits);
	
	predict.print(argv[2]);
	return 0;
}
