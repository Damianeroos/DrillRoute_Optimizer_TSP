#include "alg_two_opt.h"

TspResult AlgTwoOpt::solve(TspProblem& tsp, callback){
    QVector<int> permutation;

    permutation.reserve(tsp.size() + 1);

    //starting algorithm
    for(int i=1;i<=tsp.size();i++){
        permutation.push_back(i);
    }

    //tsp.permutation() = permutation;
    TspResult rval(std::move(permutation), tsp.computeDistance());
    return rval;
}

