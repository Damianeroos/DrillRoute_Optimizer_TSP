#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgNP : public IAlgorithm
{
public:
    TspResult solve(TspProblem& tsp, callback) override;

};


