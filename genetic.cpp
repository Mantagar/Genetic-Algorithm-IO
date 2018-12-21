#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <mpi.h>
#include <vector>
#include <cmath>
#include "optim_functions.h"
#include <fstream>
#include <chrono>

int mpi_size;
int mpi_rank;
MPI_Comm comm = MPI_COMM_WORLD;

using namespace std;
class Island {

  int dim, size;
  vector<vector<double>> population;
  vector<double> scores;
  double (*initFunc)();
  double (*fitFunc)(int, double*);
  int idx1;
  int idx2;
  vector<double> std;
  vector<double> mean;

  void eval() {
    for(int s=0; s<size; s++)
        scores[s] = fitFunc(dim, &population[s][0]);
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

  void init() {
    for(int s=0; s<size; s++)
      for(int d=0; d<dim; d++)
        population[s][d] = initFunc();
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

  vector<double> getRandomRepresentative() {
    int id = rand()%size;
    return population[id];
  }

  void addToPopulation(vector<double> rep) {
    int id = 0;
    for(int i=1; i<size; i++)
      if(scores[id]<scores[i]) id = i;
    population[id] = rep;
  }

  void updateMetrics() {
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

  vector<double> getMean() {
    return mean;
  }

  vector<double> getStd() {
    return std;
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

  int dim = 1000;//atoi(argv[1]);
  int populationSize = 25;//atoi(argv[2]);
  double mutationProb = 0.001;//atof(argv[3]);
  int problem = 2;//atoi(argv[4]);

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

  vector<int> links;
  string topology(argv[1]);

  if(topology=="ring") {
    int node = mpi_rank+1;
    if(node==mpi_size) node = 0;
    links.push_back(node);
    node = mpi_rank-1;
    if(node==-1) node = mpi_size-1;
    links.push_back(node);
  }
  else if(topology=="torus") {
    int width = atoi(argv[2]);
    if(mpi_size%width){
      if(mpi_rank==0) cerr << "Incompatible mpi_size" << endl;
      MPI_Finalize();
      return 0;
    }
    int height = mpi_size/width;
    int x = mpi_rank%width;
    int y = mpi_rank/width;
    int nodeX = x+1;
    if(nodeX==width) nodeX = 0;
    links.push_back(y*width+nodeX);
    nodeX = x-1;
    if(nodeX==-1) nodeX = width-1;
    links.push_back(y*width+nodeX);
    int nodeY = y+1;
    if(nodeY==height) nodeY = 0;
    links.push_back(nodeY*width+x);
    nodeY = y-1;
    if(nodeY==-1) nodeY = height-1;
    links.push_back(nodeY*width+x);
  }
  else if(topology=="full") {
    for(int i=0; i<mpi_size; i++)
      if(i!=mpi_rank)
        links.push_back(i);
  }
  else if(topology=="topology") {
    ifstream linkfile(argv[2]);
    int a, b;
    while(linkfile >> a >> b)
      if(a==mpi_rank) links.push_back(b);
  }
  else {
    if(mpi_rank==0) cerr << "Unknown topology setup" << endl;
    MPI_Finalize();
    return 0;
  }

  MPI_Request send_req[links.size()];
  MPI_Request recv_req[links.size()];

  vector<vector<double>> immigrants;
  immigrants.resize(links.size());
  for(int i=0; i<links.size(); i++)
    immigrants[i].resize(dim);

  vector<double> log;
  log.resize(mpi_size);

  Island island(dim, populationSize, initFunc, fitFunc);

  long counter = 0;
  auto start = chrono::high_resolution_clock::now();
  for(int r=0; r<200000; r++) {
    counter++;

    island.next(mutationProb);

    if(counter%100==0) {
      double score = island.getBestScore();
      MPI_Gather(&score, 1, MPI_DOUBLE, &log[0], 1, MPI_DOUBLE, 0, comm);
      if(mpi_rank==0)      
        for(int i=0; i<mpi_size; i++)
          cout << i << ":\t" << log[i] << endl;
    }

    if(counter%200==0) {
      for(int i=0; i<links.size(); i++) {
        vector<double> rep = island.getRandomRepresentative();
        MPI_Isend(&rep[0], dim, MPI_DOUBLE, links[i], 0, comm, &send_req[i]);
      }

      for(int i=0; i<links.size(); i++)
        MPI_Irecv(&immigrants[i][0], dim, MPI_DOUBLE, links[i], 0, comm, &recv_req[i]);

      for(int i=0; i<links.size(); i++) {
        MPI_Wait(&recv_req[i], MPI_STATUS_IGNORE);
        island.addToPopulation(immigrants[i]);
      }
    }

    if(counter%5000==0) {
      island.updateMetrics();
      vector<double> std = island.getStd();
      vector<double> mean = island.getMean();

      vector<double> meanMetrics;
      meanMetrics.resize(dim*mpi_size);
      MPI_Allgather(&mean[0], dim, MPI_DOUBLE, &meanMetrics[0], dim, MPI_DOUBLE, comm);

      int similar;
      bool reset = false;
      for(int s=0; s<mpi_rank; s++) {
        similar = 0;
        for(int d=0; d<dim; d++)
          if(mean[d]-std[d]*3<meanMetrics[s*dim+d] && meanMetrics[s*dim+d]<mean[d]+std[d]*3)
            similar++;
        if(similar!=0)cout << similar << endl;
        if(similar>dim*0.5) {
          reset = true;
          break;
        }
      }
            
      if(reset) {
        island.init();
        cout << string(mpi_rank+": RESET") << endl;     
      }
    }

  }
  MPI_Barrier(comm);
  auto end = chrono::high_resolution_clock::now();
  chrono::duration<double> diff = end-start;
  if(mpi_rank==0)
    cout << "TIME:\t" << diff.count() << endl;
  MPI_Finalize();
  return 0;
}
