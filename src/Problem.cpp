#include "Problem.h"

Problem::Problem(std::function<double()> init_function_init,
                 std::function<double(int, double*)> fit_function_init)
                 : init_function{std::move(init_function_init)},
                   fit_function{std::move(fit_function_init)}
{
}

double Problem::initFunction() {
    return init_function();
}

double Problem::fitFunction(int dim, double *point) {
    return fit_function(dim, point);
}
