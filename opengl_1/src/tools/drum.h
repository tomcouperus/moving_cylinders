#ifndef DRUM_H
#define DRUM_H


#include <QOpenGLDebugLogger>

#include "../vertex.h"
#include "tool.h"

class Drum : public Tool
{
    float r, rho;
public:
    Drum();
    Drum(float curveRadius, float midRadius, float height, int sectors, QVector3D position);

    inline void setRadius(float radius) {r=radius;}
    inline float getRadius(){ return r; }

    inline void setCurvatureRadius(float curveRadius) {rho=curveRadius;}
    inline float getCurvatureRadius(){ return rho; }

    float getRadiusAt(float a) override;
    float getRadiusDaAt(float a) override;
    float getSphereCenterHeightAt(float a) override;
    float getSphereCenterHeightDaAt(float a) override;
    float getSphereRadiusAt(float a) override;
    float getSphereRadiusDaAt(float a) override;

    Vertex getToolSurfaceAt(float a, float tRad) override;

private:
    inline float D() {return rho-r;}

    float getAngleAt(float a);
    float getAngleDaAt(float a);
};

#endif // DRUM_H
