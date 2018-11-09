#include <iostream>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <math.h>

#define PI 3.1415926535
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
      if(scores[idx1]>=scores[i]) idx1 = i;
    idx2 = (idx1==0) ? 1 : 0;
    for(int i=0; i<size; i++)
      if(scores[idx2]>=scores[i] and i!=idx1) idx2 = i;
    cout << idx1<<" "<<idx2 << endl;
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
          if(rand()/(double)RAND_MAX<mutationProb) population[s][d] = initFunc(d); //Random mutation of genes with given probability
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
      select();
      crossover();
      mutate();
      eval();
      printFittestScore();
    } while(shouldContinue());
  }

  void printFittestScore() {
    cout << "Best score: " << scores[idx1] << endl;
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

double rastriginInit(int dim_id) {
  return (double)rand() / RAND_MAX * 10.24 - 5.12;
}

double rastrigin(double *point, int DIMENSION){
  double result = DIMENSION*10;
  for(int i=0; i<DIMENSION; i++){
    result += point[i]*point[i]-10*cos(2*PI*point[i]);
  }
  return result;
}

int main(int argc, char** argv) {
  if(argc<4) {
    cout << "Usage: " << argv[0] << " dim size mutationProb" << endl;
    exit(1);
  }
  int dim = atoi(argv[1]);
  int size = atoi(argv[2]);
  double mutationProb = atof(argv[3]);
  Genetic instance(dim, size, mutationProb, rastriginInit, rastrigin);
  instance.run();
  instance.print();

  return 0;
}
