#ifndef TOOL_H
#define TOOL_H
#include <QOpenGLDebugLogger>

#include "vertex.h"

class Tool
{
protected:
    float height = 2;

    constexpr static float PI = acos(-1.0f);

    QVector3D posit;
    float a0=0;
    float a1=1;

    int sectors = 20;
    QVector3D axisVector = QVector3D(0.0,0.0,1.0);
    QVector3D perpVector = QVector3D(1.0,0.0,0.0); // a vector perpendicular to the axis
    QVector<Vertex> vertexArr;
public:
    Tool() : vertexArr(), sectors(50), height(2), posit(QVector3D(0,0,0)) {}

    virtual void setSectors(int sectors) = 0;
    virtual void setHeight(float height) = 0;
    virtual void setPosit(QVector3D position) = 0;
    void setA0(float a0) {this->a0 = a0;}
    void setA1(float a1) {this->a1 = a1;}

    virtual float getRadiusDerivativeWRTa(float a) = 0;
    virtual float getRadiusAt(float a) = 0;

    inline float getA0(){ return a0; }
    inline float getA1(){ return a1; }
    inline float getHeight(){ return height; }

    inline QVector3D getPosition() {return posit; }

    inline QVector3D getAxisVector() {return axisVector; }
    inline QVector3D getVectorPerpToAxis() {return perpVector; }
    inline int getSectors(){ return sectors; }
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
};
#endif // TOOL_H
