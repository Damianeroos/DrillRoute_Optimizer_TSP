#pragma once

#include <functional>

#include "ialgorithm.h"



class AlgSA : public IAlgorithm
{
public:  
    TspResult solve(const TspProblem& tsp, callback) override;

};


