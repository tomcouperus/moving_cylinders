#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "cylinder.h"

class Envelope
{
    Cylinder cylinder;
    // P(t) = at³+bt²+ct+d
    float a, b, c, d;
    // TODO: A w.r.t. time t
    QVector3D axisDirection;
    float t0, t1;
    QVector<Vertex> vertexArr;
public:
    Envelope();
    Envelope(Cylinder cylinder, float a, float b, float c, float d, QVector3D direction);

    void initEnvelope();
    void setCylinder(Cylinder cylinder);
    void setSectors(int sectors);
    void setPath(float a, float b, float c, float d);
    void setAxisDirection(QVector3D direction);

    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
private:
    int sectors;
    void computeEnvelope();
    Vertex calcSurfaceVertex(float t, float a);
};

#endif // ENVELOPE_H
