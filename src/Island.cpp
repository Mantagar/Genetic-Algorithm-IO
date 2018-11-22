#include <cstdlib>
#include "Island.h"

Island::Island(int dim, int size, std::unique_ptr<Problem> problemInit)
{
    this->dim = dim;
    this->size = size;
    this->population = new double*[size];
    this->scores = new double[size];
    this->problem = std::move(problemInit);
    for(int i=0; i<size; i++)
        population[i] = new double[dim];

    init();
    eval();
    select();
}

Island::~Island() {
    delete[] scores;
    for(int i=0; i<size; i++)
        delete[] population[i];
    delete[] population;
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

void Island::getRandomRepresentative(double* rep) {
    int id = rand()%size;
    for(int i=0; i<dim; i++)
        rep[i] = population[id][i];
}

void Island::addToPopulation(double *rep) {
    int id = 0;
    for(int i=1; i<size; i++)
        if(scores[id]<scores[i]) id = i;
    for(int i=0; i<dim; i++)
        population[id][i] = rep[i];
}

void Island::init() {
    for(int s=0; s<size; s++)
        for(int d=0; d<dim; d++)
            population[s][d] = problem->initFunction();
}

void Island::eval() {
    for(int s=0; s<size; s++)
        scores[s] = problem->fitFunction(dim, population[s]);
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
                if(rand()/(double)RAND_MAX<mutationProb) population[s][d] = problem->initFunction();
}
