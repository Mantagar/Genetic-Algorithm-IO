#include <iostream>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
using namespace std;
/* Let's use Java's naming convention */
class Genetic {

  int dim, size;
//list usage may be questionable (vectors?) 
  list<double*> population;
  double (*initFunc)(int);

  void init() {
//this may lead to memory leaks if not handled properly in select function
    while(!population.empty()) {
      double* sample = population.front();
      population.pop_front();
      delete sample;
    }
    for(int s=0; s<size; s++) {
      double* sample = new double[dim];
      for(int d=0; d<dim; d++)
        sample[d] = initFunc(d);
      population.push_front(sample);
    }
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

  Genetic(int dim, int size, double (*initFunc)(int)) {
    srand(time(NULL));
    this->dim = dim;
    this->size = size;
    this->initFunc = initFunc;
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
    for(double* const& sample : population) {
      cout << setprecision(5) << fixed << sample[0];
      for(int i=1; i<dim; i++)
        cout << ",\t" << sample[i];
      cout << endl;
    }
  }

};

double sampleInitFunc(int dim_id) {
  return (double)rand() / RAND_MAX * 10 - 5;
}

int main() {
  Genetic instance(3, 100, sampleInitFunc);
  instance.run();
  instance.print();

  return 0;
}
