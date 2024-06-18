#include "envelope.h"

Envelope::Envelope() :
    adjEnv(nullptr),
    toolMovement(CylinderMovement()),
    tool()
{
    sectorsA = 200;
    sectorsT = 200;
    qDebug() << "Envelope()";
}

Envelope::Envelope(CylinderMovement toolMovement, Tool *tool) :
    adjEnv(nullptr),
    toolMovement(toolMovement),
    tool(tool)
{
    sectorsA = tool->getSectors();
    sectorsT = toolMovement.getPath().getSectors();
}

Envelope::Envelope(CylinderMovement toolMovement, Tool *tool, Envelope *adjEnvelope) :
    adjEnv(adjEnvelope),
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
}

void Envelope::setToolMovement(CylinderMovement toolMovement)
{
    this->toolMovement = toolMovement;
    sectorsT = toolMovement.getPath().getSectors();

    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

void Envelope::setTool(Tool *tool)
{
    this->tool = tool;
    sectorsA = tool->getSectors();
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

void Envelope::setAdjacentEnvelope(Envelope *env){
    this->adjEnv = env;
    computeEnvelope();
}

void Envelope::computeToolCenters()
{
    vertexArrCenters.clear();
    endCurveArr.clear();

    QVector3D color = QVector3D(0,0,1);

    QVector3D v1, v2;
    SimplePath path = toolMovement.getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1();
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            v1 = calcToolCenterAt(t, a);
            v2 = calcToolCenterAt(t, a+aDelta);

            // Add vertices to array
            vertexArrCenters.append(Vertex(v1, color));
            vertexArrCenters.append(Vertex(v2, color));

            if(a >= a1-aDelta){
                endCurveArr.append(v2);
            }
        }
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
    float t1 = path.getT1();
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
    float t1 = path.getT1();
    QVector<Vertex> normals;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        normals.clear();
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            p1 = calcToolCenterAt(t,a);
            p2 = calcToolCenterAt(t, a+aDelta);
            v1 = p1 + tool->getRadiusAt(a)*computeNormal(t, a, contToAdj);
            v2 = p2 + tool->getRadiusAt(a)*computeNormal(t, a+aDelta, contToAdj);

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
    QVector3D axis = calcToolAxisDirecAt(t);
    // s is offset by a0 wrt formula because path is coded at the base of the cone/tool instead of
    // at the base of the axis of centers
    float a0 = tool->getA0();
    QVector3D center = getPathAt(t) + (a)*(axis);

    return center;
}

QVector3D Envelope::getPathAt(float t){
    if (adjEnv != nullptr && contToAdj){
        QVector3D adjCenter = adjEnv->endCurveArr[adjEnv->toolMovement.getPath().getIdxAtTime(t)];
        float rAdjEnv = adjEnv->tool->getRadiusAt(adjEnv->tool->getA1());
        float rEnv = tool->getRadiusAt(tool->getA0());
        float a0 = tool->getA0();
        QVector3D axis = calcToolAxisDirecAt(t);
        return adjCenter + (rAdjEnv - rEnv)*computeNormal(t,adjEnv->tool->getA1(),adjEnv) - (a0)*(axis);
    } else {
        SimplePath path = toolMovement.getPath();
        return path.getPathAt(t);
    }
}

QVector3D Envelope::getPathDirecAt(float t){
    int idx = toolMovement.getPath().getIdxAtTime(t);
    if(idx = adjEnv->endCurveArr.size()-1) {
        return (adjEnv->endCurveArr[idx] - adjEnv->endCurveArr[idx-1]);
    } else {
        return (adjEnv->endCurveArr[idx+1] - adjEnv->endCurveArr[idx]);
    }
}

QVector3D Envelope::calcToolAxisDirecAt(float t)
{
    QVector3D axis = toolMovement.getAxisDirectionAt(t);
    if (adjEnv != nullptr && contToAdj){
        QMatrix4x4 rotation = getAdjMovementRotation(t);
        QVector4D axisDirecNew = rotation * QVector4D(axis,0);
        axis = QVector3D(axisDirecNew.x(),axisDirecNew.y(),axisDirecNew.z());
    }
    return axis;
}

QVector3D Envelope::calcAxisRateOfChange(float t)
{
    QVector3D axis = toolMovement.getAxisRateOfChange(t).normalized();
    if (adjEnv != nullptr && contToAdj){
        QMatrix4x4 rotation = getAdjMovementRotation(t);
        QVector4D axisDirecNew = rotation * QVector4D(axis,0);
        axis = QVector3D(axisDirecNew.x(),axisDirecNew.y(),axisDirecNew.z());
    }
    return axis;
}

QMatrix4x4 Envelope::getAdjMovementRotation(float time)
{
    QVector3D n = adjEnv->computeNormal(time,adjEnv->tool->getA1(),false);
    QMatrix4x4 rotation;
    rotation.setToIdentity();
    float angle = 0; // qRadiansToDegrees(0.5);
    if(angle != 0) {
        rotation.rotate(angle, n);
    }
    return rotation;
}

Vertex Envelope::calcEnvelopeAt(float t, float a)
{
    QVector3D color = QVector3D(1,0,0);

    QVector3D center = calcToolCenterAt(t, a);

    float r = tool->getRadiusAt(a) + 0.0001; // + 0.0001 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a, contToAdj);
    QVector3D posit = center + r*normal;

    return Vertex(posit, normal);
}

QVector3D Envelope::calcGrazingCurveAt(float t, float a)
{
    QVector3D center = calcToolCenterAt(t, a);
    float r = tool->getRadiusAt(a) + 0.0002; // + 0.0002 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a, contToAdj);
    QVector3D posit = center + r*normal;

    return posit;
}

QVector3D Envelope::computeNormal(float t, float a, bool cont)
{
    SimplePath path = toolMovement.getPath();
    QVector3D sa = calcToolAxisDirecAt(t);
    float a0 = tool->getA0();
    QVector3D st = path.getTangentAt(t).normalized() + (a-a0)*calcAxisRateOfChange(t);
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
    QVector4D x = (mInv*b);
    alpha = x.x();
    beta = x.y();
    //    qDebug() << "A11"<< mInv.column(0).normalized().x();
    gamma = sqrt(1-ra*ra*(mInv.column(0).x())); // abs to solve issue with imaginaries?
    //    qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma;

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    // qDebug() << "norm: " << normal;
    normal.normalize();
    return normal;
}
