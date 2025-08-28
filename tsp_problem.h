
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
        startPoint_.setX(START_POINT_X);
        startPoint_.setY(START_POINT_Y);
    }
    void clear();

    QVector<double>& pointX();

    QVector<double>& pointY();



    QPointF &startPoint();

    QVector<QVector<double> >& dist();

private:
    QVector<int> permutation_;
    QVector<QVector<double>> dist_;
    QVector<QPoint> points_;
    QVector<double> pointX_, pointY_;
    QPointF startPoint_;
};

#endif // TSPPROBLEM_H
