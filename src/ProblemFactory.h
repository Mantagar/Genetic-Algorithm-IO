#pragma once

#include "ProblemName.h"
#include "Problem.h"
#include <memory>

class ProblemFactory
{
public:
    ProblemFactory() = default;

    std::unique_ptr<Problem> create(ProblemName);
};