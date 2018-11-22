#pragma once

#include <functional>
#include <memory>
#include "Problem.h"

class Island
{
public:
    Island(int dim, int size, std::unique_ptr<Problem>);
    ~Island();

    void next(double mutationProb);
    double getBestScore();
    void getRandomRepresentative(double* rep);
    void addToPopulation(double *rep);

private:
    void init();
    void eval();
    void select();
    void crossover();
    void mutate(double mutationProb);

    int dim, size;
    double** population;
    double* scores;
    std::unique_ptr<Problem> problem;
    int idx1;
    int idx2;
};
