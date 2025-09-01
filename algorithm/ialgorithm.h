#pragma once

#include <functional>

#include "../tsp_problem.h"

struct Progress {
    int current = 0;
    int total = 0;
};

using callback = std::function<void(const Progress&, const std::vector<int>* preview)>;

class IAlgorithm
{
public:
    virtual ~IAlgorithm() = default;
    virtual TspResult solve(const TspProblem& tsp, callback = {}) = 0;
};


