
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

const QVector<double> &TspProblem::pointX() const
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

QVector<int>& TspProblem::permutation()
{
    return permutation_;
}


