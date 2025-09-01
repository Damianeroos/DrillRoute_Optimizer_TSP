#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgTwoOpt : public IAlgorithm
{
public:  
    TspResult solve(TspProblem& tsp, callback) override;

};


