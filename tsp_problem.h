#pragma once

#include<QVector>
#include<QPoint>

#define START_POINT_X 0
#define START_POINT_Y 0

struct TspResult{
    QVector<int> permutation;
    double route = 0;
};

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
    const QVector<QVector<double> >& dist() const ;
    QVector<int>& permutation();
    const QVector<int>& permutation() const;

    void setPointX(const QVector<double> &newPointX);

    void setPointY(const QVector<double> &newPointY);

    void buildDistanceMatrix();

    size_t size() const { return pointX().size();}
    double computeDistance() const;

private:
    QVector<int> permutation_;
    QVector<QVector<double>> dist_;
    QVector<QPoint> points_;
    QVector<double> pointX_, pointY_;
    QPointF startPoint_;
};

