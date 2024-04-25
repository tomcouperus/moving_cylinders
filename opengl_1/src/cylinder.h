#ifndef CYLINDER_H
#define CYLINDER_H

#include <QOpenGLDebugLogger>

#include <QVector>
#include <QVector3D>
#include "vertex.h"

class Cylinder
{
    float r, angle, height;
    int sectors;
    float theta;
    constexpr static float PI = acos(-1.0f);
    QVector3D posit;
    QVector<Vertex> vertexArr;
public:
    Cylinder();
    Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position);
    void initCylinder();
    inline void setSectors(int sectors){ sectors = sectors; theta = 2*PI/sectors; }
    inline void setRadius(float radius){ r = radius; }
    inline void setAngle(float angle){ angle = angle; }
    inline void setHeight(float height){ height = height; }
    inline void setPosit(QVector3D position){ posit = position; }

    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
private:
    QVector3D calcCircleBound(QVector3D posit, float r, float angle, float h);
};

#endif // CYLINDER_H
