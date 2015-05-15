#include "Predictor.h"

void Predictor::readFromFile(std::string fileName){
	std::string address;
	std::string value;
	std::ifstream infile(fileName.c_str());	
	if(infile == NULL){
		std::cerr << "CANNOT ACCESS FILE: " << fileName << std::endl;
		exit(1);
	}
	numberOfBranches = 0;
	while(infile >> address && infile >> value){
		std::stringstream ss;
		data tmp;
		//Count the total number of branch 
		numberOfBranches++;
		//get substring without 0x
		address = address.substr(2);
		ss << address;
		ss >> std::hex >> tmp.address;
		if(value.compare("NT") == 0)
			tmp.branchValue = 0;
		else if(value.compare("T") == 0)
			tmp.branchValue = 1;
		//Inappropriate input
		else{
			std::cerr << "Undefined input for branch value" << std::endl;
			exit(1);
		}
		info.push_back(tmp);
	}
	infile.close();
}

void Predictor::alwaysTaken(){
	unsigned long long takenCount = 0;
	for(unsigned long long i = 0; i < info.size(); i++){
		if(info[i].branchValue == 1)
			takenCount++;
	}
	result tmp;
	tmp.numberOfCorrectPredictions = takenCount;
	tmp.typeOfPrediction = "Taken";
	output.push_back(tmp);
}

void Predictor::alwaysNotTaken(){
	unsigned long long notTakenCount = 0;
	for(unsigned long long i = 0; i < info.size(); i++){
		if(info[i].branchValue == 0)
			notTakenCount++;
	}
	result tmp;
	tmp.numberOfCorrectPredictions = notTakenCount;
	tmp.typeOfPrediction = "Not Taken";
	output.push_back(tmp);
}

void Predictor::bimodalSingleBit(int tableSize){
	unsigned long long correctPrediction = 0;
	//Pre-setting -- all prediction counter set to Taken
	std::vector<unsigned int> predictionCounter(tableSize, 1);

	for(unsigned long long i = 0; i < info.size(); i++){
		int index;
		//Make sure the index will fall within the table size
		//This takes care of the case where index  >= table size	
		index = info[i].address % tableSize;
		//The prediction is correct and increment the count by one
		if(predictionCounter[index] == info[i].branchValue)
			correctPrediction++;
		//The prediction is incorrect and modify the element of the table in that specific index to the new value 
		else
			predictionCounter[index] = info[i].branchValue;
	}
	result tmp;
	tmp.numberOfCorrectPredictions = correctPrediction;
	tmp.typeOfPrediction = "Bimodal Single Bit";
	output.push_back(tmp);
}

void Predictor::bimodalTwoBits(int tableSize){
	unsigned long long correctPrediction = 0;
	//Pre-seting -- all prediction counter set to Strongly Taken
	//bits 00 or decimal 0 for Strongly Not Taken
	//bits 01 or decimal 1 for Weakly Not Taken
	//bits 10 or decimal 2 for Weakly Taken
	//bits 11 or decimal 3 for Strongly Taken
	std::vector<unsigned int> predictionCounter(tableSize, 3);
        //tableSize/16: an integer division. Every hex digit has 16 different combinations
        //Take in as many combinations as the table size allows 
	//This computes the maximum number of digits we can have with a table of size tableSize
	for(unsigned long long i = 0; i < info.size(); i++){
                int index;
		index = info[i].address % tableSize;
		unsigned int state = predictionCounter[index];
		//Shift the branchValue left by one (either 00 or 10) and compare with the second least significant bit of state
		//If equal, then we have a match, else we have false prediction
		if((info[i].branchValue << 1) == (state & 2)){
			correctPrediction++;
		}
		//Update state
		//new branch value is Taken
		if(info[i].branchValue == 1){
			//Not in Strongly Taken
			if(state != 3)
				state++;
		//New branch value is Not Taken
		}else{
			//Not in Strongly Not Taken
			if(state != 0)
				state--;
		}
		predictionCounter[index] = state;
		
        }
        result tmp;
        tmp.numberOfCorrectPredictions = correctPrediction;
        tmp.typeOfPrediction = "Bimodal Two Bit";
        output.push_back(tmp);
}

void Predictor::gshare(int grBits, int gshareCounterSize){
	//Value used to update gr later
	unsigned int grSize = (int) pow(2, grBits) - 1;
	//Count number of correct predictions
	unsigned long long correctPrediction = 0;
	//Global register declared and initialized to zero
	unsigned long long gr = 0;
	//Counter initialize to Strongly Taken
	std::vector<unsigned int> counter(gshareCounterSize, 3);
	//Going through each info element
	for(unsigned int i = 0; i < info.size(); i++){
		//XOR branch address and global register
		//And modularize by the size of the counter
		int index = (info[i].address ^ (gr & grSize)) % gshareCounterSize;

		unsigned int state = counter[index];
		//Update state
		if((info[i].branchValue << 1) == (state &2))
			correctPrediction++;
		//Current branch is Taken
		if(info[i].branchValue == 1){
			//Update if not in Strongly Taken
			if(state != 3)
				state++;
		//Current branch is Not Taken
		}else{
			//Updata if not in Strongly Not Taken
			if(state != 0)
				state--;
		}
		//Update global register
		//Eventhough gr holds more bits than we need (int variable), but we & it with the size of the register defined by the parameter
		//Therefore, this will not cause any problem
		gr <<= 1;
		gr |= info[i].branchValue;
		counter[index] = state;
	}
	result tmp;
	tmp.numberOfCorrectPredictions = correctPrediction;
	tmp.typeOfPrediction = "Gshare";
	output.push_back(tmp);
}

void Predictor::tournament(int bimodalTableSize, int gshareTableSize, int tournamentTableSize, int grBits){
	unsigned long long correctPrediction = 0;
	unsigned int grSize = (int) pow(2, grBits) -1;
	unsigned long long gr = 0;
	//Separate counter for each predictor
	std::vector<unsigned int> gshareCounter (gshareTableSize, 3);
	std::vector<unsigned int> bimodalCounter (bimodalTableSize, 3);
	//11 for preferred gshare, 10 for weakly preferred gshare, 01 for weakly preferred bimodal, and 00 for preferred bimodal
	//initially set to 11 or 3 or preferred gshare
	std::vector<unsigned int> tournamentCounter (tournamentTableSize, 3);
	for(unsigned int i = 0; i < info.size(); i++){
		int tournamentIndex = info[i].address % tournamentTableSize;
		int bimodalIndex = info[i].address % bimodalTableSize;
		int gshareIndex = (info[i].address ^ (gr & grSize)) % gshareTableSize;
		unsigned int bimodalState = bimodalCounter[bimodalIndex];	
		unsigned int gshareState = gshareCounter[gshareIndex];
		unsigned int tournamentState = tournamentCounter[tournamentIndex];
		//Load the predicted value of both predictor to a temporary variable
		//The second least significant bit determines the prediction
		unsigned int bimodalPredictedValue = bimodalState & 2;
		unsigned int gsharePredictedValue = gshareState & 2;
		//Update both bimodal and gshare state
		//Current branch is Taken
		if(info[i].branchValue == 1){
			//Update if not in Strongly Taken for both
			if(gshareState != 3)
				gshareState++;
			if(bimodalState != 3)
				bimodalState++;
		//Current branch is Not Taken
		}else{
			//Updata if not in Strongly Not Taken for both
			if(gshareState != 0)
				gshareState--;
			if(bimodalState != 0)
				bimodalState--;
		}
		//Update global register
		//Eventhough gr holds more bits than we need (int variable), but we & it with the size of the register defined by the parameter
		//in the begining of the loop. Therefore, this will not cause any problem
		gr <<= 1;
		gr |= info[i].branchValue;
		gshareCounter[gshareIndex] = gshareState;
		bimodalCounter[bimodalIndex] = bimodalState;
		//Now, determine which prediction to take
		//Both predictor provide same prediction
		//In this case, we dont need to change the state. Therefore, we can simply increment the count if the prediction is correct
		if(gsharePredictedValue == bimodalPredictedValue){
			//Since both predictors have the same prediction value, pick either one to compare with current branch value
			if((info[i].branchValue << 1) == gsharePredictedValue){
				correctPrediction++;
			}
		}
		//Different preditions from the two predictors
		else{
			//Taking gshare prediction
			if((tournamentState & 2) == 2){
				//gshare prediction is correct
				if((info[i].branchValue << 1) == gsharePredictedValue){
					correctPrediction++;
					
					if(tournamentState != 3)
						tournamentState++;
				}
				//gshare prediction is incorrect
				else{
					if(tournamentState != 0)
						tournamentState--;
				}
			}
			//Taking bimodal prediction
			else{
				//Bimodal prediction is correct
				if((info[i].branchValue << 1)== bimodalPredictedValue){
					correctPrediction++;
					if(tournamentState != 0)
						tournamentState--;
				}
				//Bimodal prediction is incorrect
				else{
					if(tournamentState != 3)
						tournamentState++;
				}
			}
		}
		tournamentCounter[tournamentIndex] = tournamentState;
	}
	result tmp;
	tmp.numberOfCorrectPredictions = correctPrediction;
	tmp.typeOfPrediction = "Tournament";
	output.push_back(tmp);
}

void Predictor::print(std::string fileName){
	std::ofstream outFile(fileName.c_str());
	//Formatting output
	for(unsigned int i = 0; i < output.size(); i++){
		if( i > 1 && i < 8){
			outFile << output[i].numberOfCorrectPredictions << "," << numberOfBranches << "; ";
		}
		else if( i > 8 && i < 15)
			outFile << output[i].numberOfCorrectPredictions << "," << numberOfBranches << "; ";
		else if( i > 15 && i < 24)
			outFile << output[i].numberOfCorrectPredictions << "," << numberOfBranches << "; ";
		else
			outFile << output[i].numberOfCorrectPredictions << "," << numberOfBranches << ";" << std::endl << std::endl;
	}
}
