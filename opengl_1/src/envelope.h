#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "cylindermovement.h"
#include <QMatrix2x2>

class Envelope
{
    CylinderMovement cylinderMovement;
    Cylinder cylinder;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
public:
    Envelope();
    Envelope(CylinderMovement cylinderMovement, Cylinder cylinder);

    void initEnvelope();
    void computeEnvelope();
    Vertex calcEnvelopeAt(float t, float a);
    QVector3D computeNormal(float t, float a);
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
    void setCylinder(Cylinder cylinder);
    void setCylinderMovement(CylinderMovement cylinderMovement);
};

#endif // ENVELOPE_H
