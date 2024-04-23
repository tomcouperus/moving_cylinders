#ifndef PYRAMID_H
#define PYRAMID_H

#include <QVector>
#include <QVector3D>
#include "vertex.h"

class Pyramid
{
    QVector<Vertex> vertexArr;
public:
    Pyramid();
    void initPyramid();
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
};

#endif // PYRAMID_H
