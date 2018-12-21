CC=mpiCC

compile: genetic.cpp optim_functions.cpp islands.cpp
	$(CC) $^ -std=c++14 -o genetic.out
