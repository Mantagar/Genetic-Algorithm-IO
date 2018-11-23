#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <mpi.h>
#include "optim_functions.h"

int mpi_size;
int mpi_rank;
MPI_Comm comm = MPI_COMM_WORLD;

using namespace std;
class Island {

  int dim, size;
  double** population;
  double* scores;
  double (*initFunc)();
  double (*fitFunc)(int, double*);
  int idx1;
  int idx2;

  void init() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        population[s][d] = initFunc();
  }

  void eval() {
    for(int s=0; s<size; s++)
        scores[s] = fitFunc(dim, population[s]);
  }

  void select() {
    idx1 = 0;
    for(int i=0; i<size; i++)
      if(scores[idx1]>=scores[i]) idx1 = i;
    idx2 = (idx1==0) ? 1 : 0;
    for(int i=0; i<size; i++)
      if(scores[idx2]>=scores[i] and i!=idx1) idx2 = i;
  }

  void crossover() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        if(s!=idx1 and s!=idx2)
          population[s][d] = (rand()%2) ? population[idx1][d] : population[idx2][d];
  }

  void mutate(double mutationProb) {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        if(s!=idx1 and s!=idx2)
          if(rand()/(double)RAND_MAX<mutationProb) population[s][d] = initFunc();
  }

public:

  Island(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*)) {
    this->dim = dim;
    this->size = size;
    population = new double*[size];
    for(int i=0; i<size; i++)
      population[i] = new double[dim];
    scores = new double[size];
    this->initFunc = initFunc;
    this->fitFunc = fitFunc;
    init();
    eval();
    select();
  }

  ~Island() {
    delete[] scores;
    for(int i=0; i<size; i++)
      delete[] population[i];
    delete[] population;
  }

  void next(double mutationProb) {
    crossover();
    mutate(mutationProb);
    eval();
    select();
  }

  double getBestScore() {
    return scores[idx1];
  }

  void getRandomRepresentative(double* rep) {
    int id = rand()%size;
    for(int i=0; i<dim; i++)
      rep[i] = population[id][i];
  }

  void addToPopulation(double *rep) {
    int id = 0;
    for(int i=1; i<size; i++)
      if(scores[id]<scores[i]) id = i;
    for(int i=0; i<dim; i++)
      population[id][i] = rep[i];
  }
};

double rangeRandom(double min, double max) {
  return (double)rand() / RAND_MAX * (max-min) + min;
}

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);
  MPI_Comm_size(comm, &mpi_size);
  MPI_Comm_rank(comm, &mpi_rank);

  cout << setprecision(10) << fixed;
  srand(time(NULL)+mpi_rank);

  int dim = atoi(argv[1]);
  int size = atoi(argv[2]);
  double mutationProb = atof(argv[3]);
  int problem = atoi(argv[4]);

  double (*initFunc)();
  double (*fitFunc)(int, double*);
  switch(problem) {
    case 0:
      fitFunc = rastrigin;
      initFunc = [] () -> double { return rangeRandom(-5.12, 5.12); };
      break;
    case 1:
      fitFunc = dejong;
      initFunc = [] () -> double { return rangeRandom(-5.12, 5.12); };
      break;
    case 2:
      fitFunc = schwefel;
      initFunc = [] () -> double { return rangeRandom(-500, 500); };
      break;
    case 3:
      fitFunc = griewangk;
      initFunc = [] () -> double { return rangeRandom(-600, 600); };
      break;
    case 4:
      fitFunc = ackley;
      initFunc = [] () -> double { return rangeRandom(-1, 1); };
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
