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

    inline void setRadius(float radius) {r=radius;}
    inline float getRadius(){ return r; }

    inline void setAngle(float angle) {this->angle=angle;}
    inline float getAngle(){ return angle; }
    
    inline float getRadiusAt(float a) override {return r + a * height * tan(angle);}
    inline float getRadiusDaAt(float a) override {return height * tan(angle);}
    inline float getSphereCenterHeightAt(float a) override {return a * height + getRadiusAt(a) * tan(angle);}
    inline float getSphereCenterHeightDaAt(float a) override {return height + getRadiusDaAt(a) * tan(angle);}
    inline float getSphereRadiusAt(float a) override {return getRadiusAt(a) / cos(angle);}
    inline float getSphereRadiusDaAt(float a) override {return getRadiusDaAt(a) / cos(angle);}

    Vertex getToolSurfaceAt(float a, float tRad) override;
};

#endif // CYLINDER_H
