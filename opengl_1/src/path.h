#ifndef PATH_H
#define PATH_H

#include "vertex.h"
#include <QVector>

/**
 * @brief The Path class is a generic path class
 */
class Path
{
protected:
    QVector<Vertex> vertexArr;
    int sectors;
public:
    inline Path() : vertexArr(), sectors(50) {}
    virtual QVector3D getPathAt(float t) = 0;
    virtual void initVertexArr() = 0;

    inline void setSectors(int s) {sectors = s;}
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
};

#endif // PATH_H
