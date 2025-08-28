#include "tsp_problem.h"

#include<QtMath>

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

void TspProblem::setPointX(const QVector<double> &newPointX)
{
    pointX_ = newPointX;
}

void TspProblem::setPointY(const QVector<double> &newPointY)
{
    pointY_ = newPointY;
}

void TspProblem::buildDistanceMatrix()
{
    auto tempX = pointX_;
    auto tempY = pointY_;
    tempX.push_front(startPoint_.x());
    tempY.push_front(startPoint_.y());
    auto size = tempX.size();

    dist_.resize(size);
    for(int i = 0;i < size ; i++){
        dist_[i].resize(size);
    }

    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            dist_[i][j]=qSqrt(qPow(tempX[i]-tempX[j],2)+qPow(tempY[i]-tempY[j],2));
        }
    }
}


