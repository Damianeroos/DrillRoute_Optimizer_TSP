#pragma once

#include "tsp_problem.h"

class ITspLoader {
public:
    virtual ~ITspLoader() = default;
    virtual TspProblem load(const QString& filePath) = 0;
};

class brdTspLoader : public ITspLoader
{
public:
    TspProblem load(const QString& filePath) override;
};

