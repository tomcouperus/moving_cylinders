#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "movement/cylindermovement.h"
#include <QMatrix2x2>

class Envelope
{
    Envelope *adjEnv;
    bool contToAdj = true;
    bool positToAdj = true;
    bool reflectionLines = true;
    float reflFreq = 0.02;
    float percentBlack = 0.5;
    CylinderMovement *toolMovement;
    Tool *tool;
    double adjAxisAngle1;
    double adjAxisAngle2;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
    QVector<Vertex> vertexArrCenters;
    QVector<Vertex> vertexArrGrazingCurve;
    QVector<QVector<Vertex>> vertexArrNormals;

    QVector<QVector3D> endCurveArr;
public:
    Envelope();
    Envelope(CylinderMovement *toolMovement, Tool *tool);
    Envelope(CylinderMovement *toolMovement, Tool *tool, Envelope *adjEnvelope);

    void initEnvelope();
    void computeEnvelope();
    void computeAdjEnvelope();
    void computeToolCenters();
    void computeGrazingCurves();
    void computeNormals();

    Vertex calcEnvelopeAt(float t, float a);
    QVector3D calcToolCenterAt(float t, float a);
    QVector3D calcToolAxisDirecAt(float t);
    QVector3D calcAxisRateOfChange(float t);
    QVector3D calcGrazingCurveAt(float t, float a);
    QVector3D computeNormal(float t, float a);
    QVector3D getPathAt(float t);
    QVector3D getPathTangentAt(float t);

    QMatrix4x4 getAdjMovementRotation(float time);

    inline void setIsTanContinuous(bool value){ contToAdj = value; }
    inline void setIsPositContinuous(bool value){ positToAdj = value; }
    inline bool isTanContinuous() { return contToAdj; }
    inline void setAdjacentAxisAngles(double angle1, double angle2) { adjAxisAngle1 = angle1; adjAxisAngle2 = angle2; }
    inline QVector3D getEndCurveArrAt(int idx){ return endCurveArr[idx]; }
    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
    inline QVector<Vertex> getVertexArrCenters(){ return vertexArrCenters; }
    inline QVector<Vertex> getVertexArrGrazingCurve(){ return vertexArrGrazingCurve; }
    inline QVector<Vertex> getVertexArrNormalsAt(int idx){ return vertexArrNormals[idx]; }
    inline QVector<Vertex> getVertexArrNormalsAt(float t){ SimplePath path = toolMovement->getPath();
                                                            return vertexArrNormals[path.getIdxAtTime(t)]; }
    
    inline Tool* getTool(){ return tool; }
    void setTool(Tool *tool);
    void setToolMovement(CylinderMovement *toolMovement);
    void setAdjacentEnvelope(Envelope *env);
};

#endif // ENVELOPE_H
