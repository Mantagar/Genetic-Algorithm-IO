#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <mpi.h>
#include "optim_functions.h"
#include "Island.h"
#include "utils.h"

int mpi_size;
int mpi_rank;
MPI_Comm comm = MPI_COMM_WORLD;

using namespace std;

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);

    cout << setprecision(10) << fixed;
    srand(time(NULL)+mpi_rank);

    int dim = 100;
    int size = 25;
    double mutationProb = 0.01;
    int problem = 2;

    double (*initFunc)();
    double (*fitFunc)(int, double*);

    switch(problem) {
        case 0:
            fitFunc = optimfunc::rastrigin;
            initFunc = [] () -> double { return utils::rangeRandom(-5.12, 5.12); };
            break;
        case 1:
            fitFunc = optimfunc::dejong;
            initFunc = [] () -> double { return utils::rangeRandom(-5.12, 5.12); };
            break;
        case 2:
            fitFunc = optimfunc::schwefel;
            initFunc = [] () -> double { return utils::rangeRandom(-500, 500); };
            break;
        case 3:
            fitFunc = optimfunc::griewangk;
            initFunc = [] () -> double { return utils::rangeRandom(-600, 600); };
            break;
        case 4:
            fitFunc = optimfunc::ackley;
            initFunc = [] () -> double { return utils::rangeRandom(-1, 1); };
            break;
    }

    double *gathered = new double[mpi_size*dim];
    double *rep = new double[dim];
    double *scores = new double[mpi_size];
    Island island(dim, size, initFunc, fitFunc);
    int counter = 0;

    while(true) {
        island.next(mutationProb);
        counter++;
        if(counter%100==0) {
            counter = 0;
            int reciver = rand()%mpi_size;
            island.getRandomRepresentative(rep);
            MPI_Allgather(rep, dim, MPI_DOUBLE, gathered, dim, MPI_DOUBLE, comm);
            island.addToPopulation(gathered+dim*reciver);
            double bestScore = island.getBestScore();
            MPI_Gather(&bestScore, 1, MPI_DOUBLE, scores, 1, MPI_DOUBLE, 0, comm);
            if(mpi_rank==0)
                for(int i=0; i<mpi_size; i++) {
                    cout << i << ":\t" << scores[i] << endl;
                }
        }
    }

    MPI_Finalize();

    return 0;
}
