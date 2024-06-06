#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "movement/cylindermovement.h"
#include <QMatrix2x2>

class Envelope
{
    CylinderMovement toolMovement;
    Tool *tool;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
    QVector<Vertex> vertexArrCenters;
    QVector<Vertex> vertexArrToolAxis;
    QVector<Vertex> vertexArrGrazingCurve;
    QVector<QVector<Vertex>> vertexArrNormals;
public:
    Envelope();
    Envelope(CylinderMovement toolMovement, Tool *tool);

    void initEnvelope();
    void computeEnvelope();
    void computeToolCenters();
    void computeAxis();
    void computeGrazingCurves();
    void computeNormals();

    Vertex calcEnvelopeAt(float t, float a);
    QVector3D calcToolCenterAt(float t, float a);
    QVector3D calcToolAxisAt(float t);
    QVector3D calcGrazingCurveAt(float t, float a);
    QVector3D computeNormal(float t, float a);

    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
    inline QVector<Vertex> getVertexArrCenters(){ return vertexArrCenters; }
    inline QVector<Vertex> getVertexArrToolAxis(){ return vertexArrToolAxis; }
    inline QVector<Vertex> getVertexArrGrazingCurve(){ return vertexArrGrazingCurve; }
    inline QVector<Vertex> getVertexArrNormalsAt(int idx){ return vertexArrNormals[idx]; }
    inline QVector<Vertex> getVertexArrNormalsAt(float t){ SimplePath path = toolMovement.getPath(); return vertexArrNormals[path.getIdxAtTime(t)]; }
    
    void setTool(Tool *tool);
    void setToolMovement(CylinderMovement toolMovement);
};

#endif // ENVELOPE_H
