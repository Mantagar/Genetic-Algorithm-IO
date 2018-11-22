#include <cmath>
#define _USE_MATH_DEFINES

namespace optimfunc {

double ackley(int dim, double *point) {
    double sum_result1 = 0;
    double sum_result2 = 0;
    for (int i = 0; i < dim; i++) {
        sum_result1 += point[i] * point[i];
        sum_result2 += cos(2 * M_PI * point[i]);
    }
    return -20 * exp(-0.2 * sqrt(sum_result1 / dim)) - exp(sum_result2 / dim) + 20 + exp(1.);
}

double griewangk(int dim, double *point) {
    double sum_result = 0;
    double product_result = 1;
    for (int i = 0; i < dim; i++) {
        sum_result += point[i] / 4000 * point[i];
        product_result *= cos(point[i] / sqrt((double) i + 1));
    }
    return sum_result - product_result + 1;
}

double schwefel(int dim, double *point) {
    double result = 0;
    for (int i = 0; i < dim; i++) {
        result -= point[i] * sin(sqrt(std::abs(point[i])));
    }
    return result + dim * 418.9829;
}

double dejong(int dim, double *point) {
    double result = 0;
    for (int i = 0; i < dim; i++) {
        result += point[i] * point[i];
    }
    return result;
}

double rastrigin(int dim, double *point) {
    double result = dim * 10;
    for (int i = 0; i < dim; i++) {
        result += point[i] * point[i] - 10 * cos(2 * M_PI * point[i]);
    }
    return result;
}

}