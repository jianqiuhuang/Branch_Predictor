GCC = g++
FLAGS = -g -ggdb -Wall -std=c++0x 
EXE = predictors
DIR = ./source/

all: $(DIR)Driver.o $(DIR)Predictor.o ./test_machine/test_machine.c
	$(GCC) $(FLAGS) -o $(EXE) $(DIR)Driver.o $(DIR)Predictor.o 
	gcc -Wall -g -std=c99 -o test ./test_machine/test_machine.c

Driver.o: $(DIR)Driver.cpp
	$(GCC) $(FLAGS) -c $(DIR)Driver.cpp

Predictor.o: $(DIR)Predictor.cpp
	$(GCC) $(FLAGS) -c $(DIR)Predictor.cpp

clean:
	rm -f $(DIR)*.o $(EXE) test *.txt
