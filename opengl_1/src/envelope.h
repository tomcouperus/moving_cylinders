#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "cylindermovement.h"

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
    inline void setCylinder(Cylinder cylinder) {this->cylinder = cylinder;}
    inline void setCylinderMovement(CylinderMovement cylinderMovement) {this->cylinderMovement = cylinderMovement;}
};

#endif // ENVELOPE_H
