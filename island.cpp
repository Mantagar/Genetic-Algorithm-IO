#include <vector>
using namespace std;

void Island::eval() {
  for(int s=0; s<size; s++)
    scores[s] = fitFunc(dim, &population[s][0]);
}

void Island::select() {
  idx1 = 0;
  for(int i=0; i<size; i++)
    if(scores[idx1]>=scores[i]) idx1 = i;
  idx2 = (idx1==0) ? 1 : 0;
  for(int i=0; i<size; i++)
    if(scores[idx2]>=scores[i] and i!=idx1) idx2 = i;
}

void Island::crossover() {
  for(int s=0; s<size; s++)
    for(int d=0; d<dim; d++)
      if(s!=idx1 and s!=idx2)
        population[s][d] = (rand()%2) ? population[idx1][d] : population[idx2][d];
}

void Island::mutate(double mutationProb) {
  for(int s=0; s<size; s++)
    for(int d=0; d<dim; d++)
      if(s!=idx1 and s!=idx2)
        if(rand()/(double)RAND_MAX<mutationProb) population[s][d] = initFunc();
}

Island::Island(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*)) {
  this->dim = dim;
  this->size = size;
  population.resize(size);
  for(int i=0; i<size; i++)
    population[i].resize(dim);
  scores.resize(size);
  std.resize(dim);
  mean.resize(dim);
  this->initFunc = initFunc;
  this->fitFunc = fitFunc;
  init();
  eval();
  select();
}

void Island::init() {
  for(int s=0; s<size; s++)
    for(int d=0; d<dim; d++)
      population[s][d] = initFunc();
}

void Island::next(double mutationProb) {
  crossover();
  mutate(mutationProb);
  eval();
  select();
}

double Island::getBestScore() {
  return scores[idx1];
}

vector<double> Island::getRandomRepresentative() {
  int id = rand()%size;
  return population[id];
}

void Island::addToPopulation(vector<double> rep) {
  int id = 0;
  for(int i=1; i<size; i++)
    if(scores[id]<scores[i]) id = i;
  population[id] = rep;
}

void Island::updateMetrics() {
  for(int d=0; d<dim; d++) {
    mean[d] = 0;
    for(int s=0; s<size; s++)
      mean[d] += population[s][d];
    mean[d] /= size;
  }

  for(int d=0; d<dim; d++) {
    std[d] = 0;
    for(int s=0; s<size; s++)
      std[d] += population[s][d] - mean[d];
    std[d] = sqrt(std[d]*std[d]/dim);
  }
}

vector<double> Island::getMean() {
  return mean;
}

vector<double> Island::getStd() {
  return std;
}