#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgTwoOpt : public IAlgorithm
{
public:  
    TspResult solve(const TspProblem& tsp, callback) override;

};


