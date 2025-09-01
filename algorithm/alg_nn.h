#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgNN : public IAlgorithm
{
public:  
    TspResult solve(TspProblem& tsp, callback) override;

};


