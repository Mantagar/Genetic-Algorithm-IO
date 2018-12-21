CC=mpiCC

compile: genetic.cpp optim_functions.cpp island.cpp
	$(CC) $^ -std=c++14 -o genetic.out
