#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include "optim_functions.h"

using namespace std;
class Genetic {

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
    //for now only 2 fittest samples can crossover
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
          population[s][d] = (rand()%2) ? population[idx1][d] : population[idx2][d]; //Random gene inheritance
  }

  void mutate(double mutationProb) {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        if(s!=idx1 and s!=idx2)
          if(rand()/(double)RAND_MAX<mutationProb) population[s][d] = initFunc(); //reinitialization of a mutated gene
  }

public:

  Genetic(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*)) {
    this->dim = dim;
    this->size = size;
    population = new double*[size];
    for(int i=0; i<size; i++)
      population[i] = new double[dim];
    scores = new double[size];
    this->initFunc = initFunc;
    this->fitFunc = fitFunc;
    init();
  }

  ~Genetic() {
    delete[] scores;
    for(int i=0; i<size; i++)
      delete[] population[i];
    delete[] population;
  }

  void run(int stepsBetweenChecks, double mutationProb) {    
    double bestScore;
    eval();
    select();
    do {
      bestScore = scores[idx1];
      cout << setprecision(10) << fixed << bestScore << endl; 
      for(int i=0; i<stepsBetweenChecks; i++) {
        crossover();
        mutate(mutationProb);
        eval();
        select();
      }
    } while(bestScore>scores[idx1]);
  }

};

double rangeRandom(double min, double max) {
  return (double)rand() / RAND_MAX * (max-min) + min;
}

int main(int argc, char** argv) {
  if(argc<5) {
    cout << "Usage:\n\t" << argv[0] << " [dimension] [population size] [mutation probability] [problem]" << endl;
    cout << "Example:\n\t" << argv[0] << " 10000 20 0.001 3" << endl;
    cout << "Problems:\n\t0 - rastrigin\n\t1 - dejong\n\t2 - schwefel\n\t3 - griewangk\n\t4 - ackley" << endl;
    exit(1);
  }

  srand(time(NULL));

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

  Genetic instance(dim, size, initFunc, fitFunc);
  instance.run(10000, mutationProb);

  return 0;
}
