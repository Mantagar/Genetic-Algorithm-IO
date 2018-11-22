#include "ProblemFactory.h"
#include "optim_functions.h"
#include "utils.h"
#include <functional>

std::unique_ptr<Problem> ProblemFactory::create(ProblemName fitFunc) {
    std::function<double()> init_function;
    std::function<double(int, double*)> fit_function;

    switch(fitFunc) {
        case ProblemName::Rastrigin:
            init_function = [] () -> double { return utils::rangeRandom(-5.12, 5.12); };
            fit_function = optimfunc::rastrigin;
            break;
        case ProblemName::Dejong:
            init_function = [] () -> double { return utils::rangeRandom(-5.12, 5.12); };
            fit_function = optimfunc::dejong;
            break;
        case ProblemName::Schwefel:
            init_function = [] () -> double { return utils::rangeRandom(-500, 500); };
            fit_function = optimfunc::schwefel;
            break;
        case ProblemName::Griewangk:
            init_function = [] () -> double { return utils::rangeRandom(-600, 600); };
            fit_function = optimfunc::griewangk;
            break;
        case ProblemName::Ackley:
            init_function = [] () -> double { return utils::rangeRandom(-1, 1); };
            fit_function = optimfunc::ackley;
            break;
    }

    return std::make_unique<Problem>(init_function, fit_function);
}
