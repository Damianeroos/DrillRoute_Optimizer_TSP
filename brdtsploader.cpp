#include "brdtsploader.h"



TspProblem brdTspLoader::load(const QString &filePath)
{
    throw std::runtime_error("dummy error");

    return TspProblem();
}


/*
            //making distance matrix
            tempX = tsp_.pointX();
            tempY = tsp_.pointY();
            tempX.push_front(tsp_.startPoint().x()); //add starting point (0,0)
            tempY.push_front(tsp_.startPoint().y());
            size = tempX.size();

            auto& dist = tsp_.dist();
            dist.resize(size);
            for(int i = 0;i < size ; i++){
                dist[i].resize(size);
            }

            //computing distance matrix
            for(int i=0;i<size;i++){
                for(int j=0;j<size;j++){
                    dist[i][j]=qSqrt(qPow(tempX[i]-tempX[j],2)+qPow(tempY[i]-tempY[j],2));
                }
            }
*/
