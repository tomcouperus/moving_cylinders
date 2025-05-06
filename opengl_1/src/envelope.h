#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "movement/cylindermovement.h"
#include <QMatrix2x2>
#include <QQuaternion>
#include "settings.h"

class Envelope
{
    int index;
    bool active;
    CylinderMovement toolMovement;
    Tool *tool;

    QVector<Envelope*> dependentEnvelopes;
    Envelope *adjEnv = nullptr;
    bool tanContToAdj = false;
    double adjAxisAngle1;
    double adjAxisAngle2;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
    QVector<Vertex> vertexArrCenters;
    QVector<Vertex> vertexArrGrazingCurve;
    QVector<QVector<Vertex>> vertexArrNormals;

    const Settings *settings;

public:
    Envelope(const Settings *settings, int index);
    Envelope(const Settings *settings, int index, Tool *tool);
    Envelope(const Settings *settings, int index, Tool *tool, Envelope *adjEnvelope);
    Envelope(const Settings *settings, int index, Tool *tool, CylinderMovement &movement);
    Envelope(const Settings *settings, int index, Tool *tool, CylinderMovement &movement, Envelope *adjEnvelope);

    inline int getIndex() { return index; }
    inline Envelope *getAdjEnvelope() { return adjEnv; }

    void initEnvelope();
    void update();

    void computeEnvelope();
    QVector3D getEnvelopeAt(float t, float a);
    QVector3D getEnvelopeDtAt(float t, float a);

    void computeToolCenters();

    void computeGrazingCurves();

    void computeNormals();
    QVector3D getNormalAt(float t, float a);
    QVector3D getNormalDtAt(float t, float a);

    QVector3D getPathAt(float t);
    QVector3D getPathDtAt(float t);
    QVector3D getPathDt2At(float t);

    QQuaternion calcAxisRotationAt(float t);
    QVector3D getAxisAt(float t);
    QVector3D getAxisDtAt(float t);
    QVector3D getAxisDt2At(float t);

    float getToolRadiusAt(float a);
    float getToolRadiusDaAt(float a);


    void registerDependent(Envelope *dependent);
    void deregisterDependent(Envelope *dependent);
    bool checkDependencies();

    inline bool isActive() { return active; }
    inline void setActive(bool value) { active = value; }

    inline bool isPositContinuous() { return adjEnv != nullptr; }
    inline void setIsTanContinuous(bool value){ tanContToAdj = value; }
    inline bool isTanContinuous() { return adjEnv != nullptr && tanContToAdj; }
    inline void setAdjacentAxisAngles(double angle1, double angle2) { adjAxisAngle1 = angle1; adjAxisAngle2 = angle2; }

    inline CylinderMovement& getToolMovement() { return toolMovement; }
    inline Tool* getTool(){ return tool; }
    void setTool(Tool *tool);
    void setAdjacentEnvelope(Envelope *env);

    inline QVector<Vertex>& getVertexArr(){ return vertexArr; }
    inline QVector<Vertex>& getVertexArrCenters(){ return vertexArrCenters; }
    inline QVector<Vertex>& getVertexArrGrazingCurve(){ return vertexArrGrazingCurve; }
    inline QVector<Vertex>& getVertexArrNormalsAt(int idx){ return vertexArrNormals[idx]; }
    inline QVector<Vertex>& getVertexArrNormalsAt(float t){ SimplePath path = toolMovement.getPath();
                                                            return vertexArrNormals[path.getIdxAtTime(t)]; }

    QMatrix4x4 getToolToPathTransform();
    QMatrix4x4 getToolAlongPathTransform();
    QMatrix4x4 getToolRotationTransform();
    QMatrix4x4 getToolTransform();
};

#endif // ENVELOPE_H
