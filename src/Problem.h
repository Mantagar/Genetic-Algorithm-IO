#pragma once

#include <functional>

class Problem
{
public:
    Problem(std::function<double()>, std::function<double(int, double*)>);

    double initFunction();
    double fitFunction(int, double*);

private:
    std::function<double()> init_function;
    std::function<double(int, double*)> fit_function;
};