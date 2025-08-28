
#include "tsp_problem.h"



void TspProblem::clear(){
    permutation_.clear();
    dist_.clear();
    points_.clear();
    pointX_.clear();
    pointY_.clear();
}

QVector<double>& TspProblem::pointX()
{
    return pointX_;
}

QVector<double>& TspProblem::pointY()
{
    return pointY_;
}

QPointF& TspProblem::startPoint()
{
    return startPoint_;
}

QVector<QVector<double> >& TspProblem::dist()
{
    return dist_;
}


