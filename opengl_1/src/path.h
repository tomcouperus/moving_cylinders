#ifndef PATH_H
#define PATH_H

#include "vertex.h"
#include <QVector>

/**
 * @brief The Path class is a generic path class
 */
class Path
{
    QVector<QVector3D> pathCoords;
    QVector<Vertex> vertexArr;
public:
    Path();
    QVector3D getPathAt(float t);
    void setPathCoords();
    void setVertexArr();

    inline QVector<QVector3D> getPathCoords() { return pathCoords; }
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
};

#endif // PATH_H
