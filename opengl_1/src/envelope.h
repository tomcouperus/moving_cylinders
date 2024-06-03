#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "cylindermovement.h"
#include <QMatrix2x2>

class Envelope
{
    CylinderMovement toolMovement;
    Tool *tool;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
    QVector<Vertex> vertexArrCenters;
    QVector<Vertex> vertexArrGrazingCurve;
public:
    Envelope();
    Envelope(CylinderMovement toolMovement, Tool *tool);

    void initEnvelope();
    void computeEnvelope();
    void computeToolCenters();
    void computeGrazingCurves();
    Vertex calcEnvelopeAt(float t, float a);
    QVector3D calcToolCenterAt(float t, float a);
    QVector3D calcGrazingCurveAt(float t, float a);
    QVector3D computeNormal(float t, float a);
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
    inline QVector<Vertex> getVertexArrCenters(){ return vertexArrCenters; }
    inline QVector<Vertex> getVertexArrGrazingCurve(){ return vertexArrGrazingCurve; }
    void setTool(Tool *tool);
    void setToolMovement(CylinderMovement toolMovement);
};

#endif // ENVELOPE_H
