#include "envelope.h"

Envelope::Envelope() :
    toolMovement(CylinderMovement()),
    tool()
{
    sectorsA = 200;
    sectorsT = 200;
    qDebug() << "Envelope()";
}

Envelope::Envelope(CylinderMovement toolMovement, Tool *tool) :
    toolMovement(toolMovement),
    tool(tool)
{
    sectorsA = tool->getSectors();
    sectorsT = toolMovement.getPath().getSectors();
}

void Envelope::initEnvelope()
{
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
    computeAxis();
}

void Envelope::setToolMovement(CylinderMovement toolMovement)
{
    this->toolMovement = toolMovement;
    sectorsT = toolMovement.getPath().getSectors();
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
    computeAxis();
}

void Envelope::setTool(Tool *tool)
{
    this->tool = tool;
    sectorsA = tool->getSectors();
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
    computeAxis();
}

void Envelope::computeToolCenters()
{
    vertexArrCenters.clear();

    QVector3D color = QVector3D(0,0,1);

    QVector3D v1, v2;
    SimplePath path = toolMovement.getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            v1 = calcToolCenterAt(t, a);
            v2 = calcToolCenterAt(t, a+aDelta);

            // Add vertices to array
            vertexArrCenters.append(Vertex(v1, color));
            vertexArrCenters.append(Vertex(v2, color));
        }
    }
}

void Envelope::computeAxis()
{
    vertexArrToolAxis.clear();

    QVector3D color = QVector3D(1,1,0);

    QVector3D v1, p1;
    SimplePath path = toolMovement.getPath();
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        p1 = path.getPathAt(t);
        v1 = calcToolAxisAt(t);

        // Add vertices to array
        vertexArrToolAxis.append(Vertex(p1, color));
        vertexArrToolAxis.append(Vertex(v1, color));
    }
}

void Envelope::computeGrazingCurves()
{
    vertexArrGrazingCurve.clear();

    QVector3D color = QVector3D(0,1,0);

    QVector3D v1, v2;
    SimplePath path = toolMovement.getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            v1 = calcGrazingCurveAt(t, a);
            v2 = calcGrazingCurveAt(t, a+aDelta);

            // Add vertices to array
            vertexArrGrazingCurve.append(Vertex(v1, color));
            vertexArrGrazingCurve.append(Vertex(v2, color));
        }
    }
}

void Envelope::computeEnvelope()
{
    vertexArr.clear();

    Vertex v1, v2, v3, v4;
    SimplePath path = toolMovement.getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        
        for (float a = tool->getA0(); a < a1; a+=aDelta)
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
        }
    }
}

void Envelope::computeNormals(){
    vertexArrNormals.clear();

    QVector3D c = QVector3D(0,1,0);

    QVector3D v1, v2, p1, p2;
    SimplePath path = toolMovement.getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    QVector<Vertex> normals;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        normals.clear();
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            p1 = calcToolCenterAt(t,a);
            p2 = calcToolCenterAt(t, a+aDelta);
            v1 = p1 + tool->getRadiusAt(a)*computeNormal(t, a);
            v2 = p2 + tool->getRadiusAt(a)*computeNormal(t, a+aDelta);

            // Add vertices to array
            normals.append(Vertex(p1,c));
            normals.append(Vertex(v1,c));
            normals.append(Vertex(p2,c));
            normals.append(Vertex(v2,c));
        }
        vertexArrNormals.append(normals);
    }
}

QVector3D Envelope::calcToolCenterAt(float t, float a)
{
    
    SimplePath path = toolMovement.getPath();
    // s is offset by a0 wrt formula because path is coded at the base of the cone/tool instead of
    // at the base of the axis of centers
    float a0 = tool->getA0();
    QVector3D center = path.getPathAt(t) + (a)*(toolMovement.getAxisDirectionAt(t).normalized());

    return center;
}

QVector3D Envelope::calcToolAxisAt(float t)
{
    SimplePath path = toolMovement.getPath();
    QVector3D axis = tool->getHeight()* toolMovement.getAxisDirectionAt(t).normalized();
    return path.getPathAt(t) + axis;
}

Vertex Envelope::calcEnvelopeAt(float t, float a)
{
    QVector3D color = QVector3D(1,0,0);
    
    QVector3D center = calcToolCenterAt(t, a);

    float r = tool->getRadiusAt(a) + 0.000; // + 0.0001 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a);
    QVector3D posit = center + r*normal;

    return Vertex(posit, normal);
}

QVector3D Envelope::calcGrazingCurveAt(float t, float a)
{
    QVector3D center = calcToolCenterAt(t, a);
    float r = tool->getRadiusAt(a) + 0.00; // + 0.01 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a);
    QVector3D posit = center + r*normal;

    return posit;
}

QVector3D Envelope::computeNormal(float t, float a)
{
    SimplePath path = toolMovement.getPath();
    QVector3D sa = toolMovement.getAxisDirectionAt(t).normalized();
    float a0 = tool->getA0();
    QVector3D st = path.getTangentAt(t) + (a-a0)*toolMovement.getAxisRateOfChange(t).normalized();
//    qDebug() << "st: " << st;
    QVector3D sNorm = QVector3D::crossProduct(sa, st).normalized();

    // Calculate alpha, beta, gamma
    float alpha, beta, gamma;
    float ra = tool->getRadiusDerivativeWRTa(a);
//    qDebug() << "ra: " << ra;
    QMatrix4x4 m(QVector3D::dotProduct(sa,sa), QVector3D::dotProduct(st,sa),0,0,
                              QVector3D::dotProduct(sa,st), QVector3D::dotProduct(st,st),0,0,
                                0,0,1,0,0,0,0,1);
//    qDebug() << "m" << m;
    QMatrix4x4 mInv = m.inverted(); // inverse of block matrix = matrix of inverse blocks
//    qDebug() << "mInv: " << mInv;
    QVector4D b = QVector4D(-ra, 0,0,0);
    QVector4D x = mInv*b;
    alpha = x.x();
    beta = x.y();
//    qDebug() << "A11"<< mInv.column(0).x();
    gamma = sqrt(1-ra*ra*(mInv.column(0).normalized().x())); // abs to solve issue with imaginaries?
//    qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma;

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    // qDebug() << "norm: " << normal;
    normal.normalize();
    return normal;
}
