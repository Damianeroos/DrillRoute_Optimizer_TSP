
#ifndef TSPPROBLEM_H
#define TSPPROBLEM_H

#include<QVector>
#include<QPoint>

#define START_POINT_X 0
#define START_POINT_Y 0

class TspProblem
{
public:
    TspProblem(){
        startX = START_POINT_X;
        startY = START_POINT_Y;
    }

private:
    QVector<int> permutation_;
    QVector<QVector<double>> dist_;
    QVector<QPoint> points_;
    QPoint startX, startY;
};

#endif // TSPPROBLEM_H
