#ifndef OPTIM_FUNCTIONS_H
#define OPTIM_FUNCTIONS_H
double ackley(int dim, double *point);    // [-1, 1]
double griewangk(int dim, double *point); // [-600, 600]
double schwefel(int dim, double *point);  // [-500, 500]
double dejong(int dim, double *point);    // [-5.12, 5.12]
double rastrigin(int dim, double *point); // [-5.12, 5.12]
#endif
