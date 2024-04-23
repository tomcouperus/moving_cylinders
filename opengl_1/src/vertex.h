#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex {
    float xCoord;
    float yCoord;
    float zCoord;
    float rVal;
    float gVal;
    float bVal;
    Vertex() : xCoord(0.0), yCoord(0.0), zCoord(0.0), rVal(0.0), gVal(0.0), bVal(0.0) {}
    Vertex(float xCoord, float yCoord, float zCoord, float rVal, float gVal, float bVal)
        : xCoord(xCoord), yCoord(yCoord), zCoord(zCoord), rVal(rVal), gVal(gVal), bVal(bVal) {}
    Vertex(QVector3D position, float rVal, float gVal, float bVal)
        : xCoord(position.x()), yCoord(position.y()), zCoord(position.z()), rVal(rVal), gVal(gVal), bVal(bVal) {}
    Vertex(QVector3D position, QVector3D color)
        : xCoord(position.x()), yCoord(position.y()), zCoord(position.z()), rVal(color.x()), gVal(color.y()), bVal(color.z()) {}

};

#endif // VERTEX_H
