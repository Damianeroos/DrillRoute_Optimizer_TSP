#pragma once

#include<QVector>
#include<QPoint>

#define START_POINT_X 0
#define START_POINT_Y 0

class TspProblem
{
public:
    TspProblem(){
        startPoint_.setX(START_POINT_X);
        startPoint_.setY(START_POINT_Y);
    }

    void clear();
    QVector<double>& pointX();
    const QVector<double>& pointX() const;
    QVector<double>& pointY();
    QPointF &startPoint();
    QVector<QVector<double> >& dist();
    QVector<int>& permutation();

private:
    QVector<int> permutation_;
    QVector<QVector<double>> dist_;
    QVector<QPoint> points_;
    QVector<double> pointX_, pointY_;
    QPointF startPoint_;
};

