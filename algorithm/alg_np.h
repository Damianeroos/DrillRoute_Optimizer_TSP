#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgNP : public IAlgorithm
{
public:  
    TspResult solve(const TspProblem& tsp, callback) override;

};


