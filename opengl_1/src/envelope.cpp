#include "envelope.h"

Envelope::Envelope() : 
    cylinderMovement(CylinderMovement()), 
    cylinder(Cylinder())
{
    sectorsA = cylinder.getSectors();
    sectorsT = cylinderMovement.getPath().getSectors();
}

Envelope::Envelope(CylinderMovement cylinderMovement, Cylinder cylinder) :
    cylinderMovement(cylinderMovement),
    cylinder(cylinder)
{}

void Envelope::initEnvelope()
{
    computeEnvelope();
}

void Envelope::computeEnvelope()
{
    vertexArr.clear();

    Vertex v1, v2, v3, v4;
    SimplePath path = cylinderMovement.getPath();
    float a = cylinder.getA0();
    float aDelta = (cylinder.getA1()-a)/sectorsA;
    float t = path.getT0();
    float tDelta = (path.getT1()-t)/sectorsT;
    for (int i = 0; i < sectorsT; i++)
    {
        for (int j = 0; j < sectorsA; j++)
        {
            v1 = calcEnvelopeAt(t, a);
            v2 = calcEnvelopeAt(t, a+aDelta);
            v3 = calcEnvelopeAt(t+tDelta, a);
            v4 = calcEnvelopeAt(t+tDelta, a+aDelta);

            // Add vertices to array
            vertexArr.append(v1);
            vertexArr.append(v4);
            vertexArr.append(v2);
            vertexArr.append(v1);
            vertexArr.append(v3);
            vertexArr.append(v4);

            a += aDelta;
        }
        t += tDelta;
    }
}

Vertex Envelope::calcEnvelopeAt(float t, float a)
{
    QVector3D color = QVector3D(1,0,0);
    
    float a0 = cylinder.getA0();
    SimplePath path = cylinderMovement.getPath();
    int idx = path.getIdxAtTime(t);
    QVector3D center = path.getPathAt(t) + (a-a0)*cylinderMovement.getAxisDirections()[idx];

    float r = cylinder.getRadiusAt(a);
    QVector3D normal = computeNormal(t, a);
    QVector3D posit = center + r*normal;

    return Vertex(posit, color);
}

QVector3D Envelope::computeNormal(float t, float a)
{
    SimplePath path = cylinderMovement.getPath();
    QVector3D sa = cylinderMovement.getAxisDirections().at(t);
    float a0 = cylinder.getA0();
    QVector3D st = path.getTangentAt(t) + (a-a0)*cylinderMovement.getAxisRateOfChange(t);
    QVector3D sNorm = QVector3D::crossProduct(sa, st).normalized();

    // Calculate alpha, beta, gamma
    float alpha, beta, gamma;
    float ra = cylinder.getRadiusDerivativeWRTa(); 
    

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    normal.normalize();
    return normal;
}
    
