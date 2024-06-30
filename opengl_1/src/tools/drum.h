#ifndef DRUM_H
#define DRUM_H


#include <QOpenGLDebugLogger>

#include "../vertex.h"
#include "tool.h"

class Drum : public Tool
{
    float r0, rm;
public:
    Drum();
    Drum(float curveRadius, float midRadius, float height, int sectors, QVector3D position);
    void initDrum();

    void setSectors(int sectors) override;
    void setRadius(float radius);
    void setMidRadius(float midRadius);
    void setHeight(float height) override;
    void setPosit(QVector3D position) override;

    inline float getMinR0() { return ((height/2)*(height/2) + rm*rm)/(2*rm);}

    inline float getRadiusDerivativeWRTa(float a) override {float s = sqrt(((r0-rm)*(r0-rm))+(a*a));  return -a/s;}
    inline float getRadiusAt(float a) override {return r0-sqrt(((r0-rm)*(r0-rm))+(a*a));}

    inline float getRadiusCurvature(){ return r0; }
    inline float getMidRadius(){ return rm; }
private:
    void computeDrum();
    Vertex calcCircleBound(float theta, float h);
};

#endif // DRUM_H
