#include <iostream>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
using namespace std;
/* Let's use Java's naming convention */
class Genetic {

  int dim, size;
  double mutationProb;
  double** population;
  double* scores;
  double (*initFunc)(int);
  double (*fitFunc)(double*, int);
  int idx1;
  int idx2;

  void init() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        population[s][d] = initFunc(d);
  }

  void eval() {
    for(int s=0; s<size; s++)
        scores[s] = fitFunc(population[s], dim);
  }

  bool shouldContinue() {
    return true; //TODO define stop condition
  }

  void select() {
    //for now only 2 fittest samples can crossover
    idx1 = 0;
    for(int i=0; i<size; i++)
      if(scores[idx]>s=cores[i]) idx1 = i;
    idx2 = (idx1==0) ? 1 : 0;
    for(int i=0; i<size; i++)
      if(scores[idx]>=scores[i] and i!=idx2) idx2 = i;
  }

  void crossover() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        if(s!=idx1 and s!=idx2)
          population[s][d] = (rand()%2) ? population[idx1][d] : population[idx2][d]; //Random gene inheritance
  }

  void mutate() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        if(s!=idx1 and s!=idx2)
          if(rand()/(double)MAX_RAND<mutationProb) population[s][d] = initFunc(d); //Random mutation of genes with given probability
  }

public:

  Genetic(int dim, int size, double mutationProb, double (*initFunc)(int), double (*fitFunc)(double*, int)) {
    srand(time(NULL));
    this->dim = dim;
    this->size = size;
    this->mutationProb = mutationProb;
    population = new double*[size];
    for(int i=0; i<size; i++)
      population[i] = new double[dim];
    scores = new double[size];
    this->initFunc = initFunc;
    this->fitFunc = fitFunc;
  }

  ~Genetic() {
    delete[] scores;
    for(int i=0; i<dim; i++)
      delete[] population[i];
    delete[] population;
  }

  void run() {
    init();
    eval();
    do {
      printMeanScore();
      select();
      crossover();
      mutate();
      eval();
    } while(shouldContinue());
  }

  void printMeanScore() {
    double meanScore = 0;
    for(int s=0; s<size; s++) {
      meanScore += scores[s];
    }
    meanScore /= size;
    cout << "Mean score: " << meanScore << endl;
  }
    
  void print() {
    for(int s=0; s<size; s++) {
      cout << setprecision(5) << fixed << population[s][0];
      for(int d=1; d<dim; d++)
        cout << ",\t" << population[s][d];
      cout << endl;
    }
  }

};

double sampleInitFunc(int dim_id) {
  return (double)rand() / RAND_MAX * 10 - 5;
}

double sampleFitFunc(double* sample, int dim) {
  double fitness = 0;
  for(int i=0; i<dim; i++) {
    fitness += sample[i];
  }
  return fitness/dim;
}

int main() {
  Genetic instance(3, 100, 0.01, sampleInitFunc, sampleFitFunc);
  instance.run();
  instance.print();

  return 0;
}
