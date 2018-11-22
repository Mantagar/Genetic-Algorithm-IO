#pragma once

class Island
{
public:
    Island(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*));
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
    double (*initFunc)();
    double (*fitFunc)(int, double*);
    int idx1;
    int idx2;
};