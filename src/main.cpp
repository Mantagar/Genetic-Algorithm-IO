#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <mpi.h>
#include "optim_functions.h"
#include "Island.h"
#include "ProblemName.h"
#include "utils.h"
#include "ProblemFactory.h"
#include <random>

int mpi_size;
int mpi_rank;
MPI_Comm comm = MPI_COMM_WORLD;

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);

    std::cout << std::setprecision(10) << std::fixed;
    srand(time(NULL)+mpi_rank);

    int dim = 100;
    int size = 25;
    double mutationProb = 0.01;
    int counter = 0;

    double *gathered = new double[mpi_size*dim];
    double *rep = new double[dim];
    double *scores = new double[mpi_size];

    ProblemFactory problemFactory;
    std::unique_ptr<Problem> problem = problemFactory.create(ProblemName::Schwefel);
    Island island(dim, size, std::move(problem));

    while(true) {
        island.next(mutationProb);
        counter++;
        if(counter%100 == 0) {
            counter = 0;
            int reciver = rand()%mpi_size;
            island.getRandomRepresentative(rep);
            MPI_Allgather(rep, dim, MPI_DOUBLE, gathered, dim, MPI_DOUBLE, comm);
            island.addToPopulation(gathered+dim*reciver);
            double bestScore = island.getBestScore();
            MPI_Gather(&bestScore, 1, MPI_DOUBLE, scores, 1, MPI_DOUBLE, 0, comm);
            if(mpi_rank == 0)
                for(int i=0; i<mpi_size; i++) {
                    std::cout << i << ":\t" << scores[i] << std::endl;
                }
        }
    }

    MPI_Finalize();

    return 0;
}
