#include <iostream>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
using namespace std;
/* Let's use Java's naming convention */
class Genetic {

  int dim, size;
  double** population;
  double* scores;
  double (*initFunc)(int);
  double (*fitFunc)(double*, int);

  void init() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        population[s][d] = initFunc(d);
  }

  void eval() {

  }

  bool shouldContinue() {
    return false;
  }

  void select() {

  }

  void crossover() {

  }

  void mutate() {

  }

public:

  Genetic(int dim, int size, double (*initFunc)(int), double (*fitFunc)(double*, int)) {
    srand(time(NULL));
    this->dim = dim;
    this->size = size;
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
      select();
      crossover();
      mutate();
      eval();
    } while(shouldContinue());
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
  Genetic instance(3, 100, sampleInitFunc, sampleFitFunc);
  instance.run();
  instance.print();

  return 0;
}
