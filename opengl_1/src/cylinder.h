#ifndef CYLINDER_H
#define CYLINDER_H

#include <QOpenGLDebugLogger>

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
    void setSectors(int sectors);
    void setRadius(float radius);
    void setAngle(float angle);
    void setHeight(float height);
    void setPosit(QVector3D position);
    inline int getSectors(){ return sectors; }
    inline float getRadius(){ return r; }
    inline float getAngle(){ return angle; }
    inline float getHeight(){ return height; }
    inline QVector3D getPosition() {return posit; }

    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
private:
    void computeCylinder();
    Vertex calcCircleBound(QVector3D posit, float r1, float theta, float h);
};

#endif // CYLINDER_H
