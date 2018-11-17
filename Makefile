CC=mpiCC

compile: genetic.cpp optim_functions.cpp
	$(CC) $^ -std=c++14 -o genetic.out

test:
	mpiexec -np 100 genetic.out
