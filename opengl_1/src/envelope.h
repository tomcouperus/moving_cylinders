#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "movement/cylindermovement.h"
#include <QMatrix2x2>
#include "settings.h"

class Envelope
{
    bool active;
    Envelope *adjEnv = nullptr;
    bool contToAdj = false;
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
    const Settings *settings;

    QVector<Envelope*> dependentEnvelopes;

public:
    Envelope(const Settings *settings);
    Envelope(const Settings *settings, CylinderMovement *toolMovement, Tool *tool);
    Envelope(const Settings *settings, CylinderMovement *toolMovement, Tool *tool, Envelope *adjEnvelope);

    void initEnvelope();
    void update();
    void computeEnvelope();
    void computeAdjEnvelope();
    void computeToolCenters();
    void computeGrazingCurves();
    void computeNormals();

    void registerDependent(Envelope *dependent);
    void deregisterDependent(Envelope *dependent);
    bool checkDependencies();

    Vertex calcEnvelopeAt(float t, float a);
    QVector3D calcToolCenterAt(float t, float a);
    QVector3D calcToolAxisDirecAt(float t);
    QVector3D calcAxisRateOfChange(float t);
    QVector3D calcGrazingCurveAt(float t, float a);
    QVector3D computeNormal(float t, float a);
    QVector3D getPathAt(float t);
    QVector3D getPathTangentAt(float t);

    QMatrix4x4 getAdjMovementRotation(float time);

    inline bool isActive() { return active; }
    inline void setActive(bool value) { active = value; }

    inline void setIsTanContinuous(bool value){ contToAdj = value; }
    inline bool isTanContinuous() { return contToAdj; }
    inline bool isPositContinuous() { return adjEnv != nullptr; }
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
