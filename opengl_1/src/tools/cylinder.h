#ifndef CYLINDER_H
#define CYLINDER_H

#include <QOpenGLDebugLogger>

#include "../vertex.h"
#include "tool.h"

class Cylinder : public Tool
{
    float r, angle;
public:
    Cylinder();
    Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position);
    void initCylinder();

    void setSectors(int sectors) override;
    void setRadius(float radius);
    void setAngle(float angle);
    void setHeight(float height)override;
    void setPosit(QVector3D position)override;
    void update() override;
    
    inline float getRadiusDaAt(float a) override {return sin(angle);}
    inline float getRadiusAt(float a) override {return (r*cos(angle) + a*sin(angle));}

    inline float getRadius(){ return r; }
    inline float getAngle(){ return angle; }
private:
    void computeCylinder();
    Vertex calcCircleBound(float r1, float theta, float h);
};

#endif // CYLINDER_H
