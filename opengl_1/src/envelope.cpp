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
{
    sectorsA = cylinder.getSectors();
    sectorsT = cylinderMovement.getPath().getSectors();
}

void Envelope::initEnvelope()
{
    computeEnvelope();
}

void Envelope::setCylinderMovement(CylinderMovement cylinderMovement)
{
    this->cylinderMovement = cylinderMovement;
    sectorsT = cylinderMovement.getPath().getSectors();
    computeEnvelope();
}

void Envelope::setCylinder(Cylinder cylinder)
{
    this->cylinder = cylinder;
    sectorsA = cylinder.getSectors();
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
        a = cylinder.getA0();
        for (int j = 0; j < sectorsA; j++)
        {
            qDebug() << "t: " << t << " a: " << a;
            v1 = calcEnvelopeAt(t, a);
            v2 = calcEnvelopeAt(t, a+aDelta);
            v3 = calcEnvelopeAt(t+tDelta, a);
            v4 = calcEnvelopeAt(t+tDelta, a+aDelta);

//            qDebug() << "v1: " << v1.getPosition();
//            qDebug() << "v2: " << v2.getPosition();
//            qDebug() << "v3: " << v3.getPosition();
//            qDebug() << "v4: " << v4.getPosition();

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

    // qDebug() << "position: " << posit;
    // qDebug() << "center:" << center;
    // qDebug() << "normal: " << normal;
    // qDebug() << "radius: " << r;

    return Vertex(posit, color);
}

QVector3D Envelope::computeNormal(float t, float a)
{
    SimplePath path = cylinderMovement.getPath();
    QVector3D sa = cylinderMovement.getAxisDirections()[t];
    float a0 = cylinder.getA0();
//    qDebug() << "a-a0: " << a-a0;
    QVector3D st = path.getTangentAt(t) + (a-a0)*cylinderMovement.getAxisRateOfChange(t);
//    qDebug() << "st: " << st;
    QVector3D sNorm = QVector3D::crossProduct(sa, st).normalized();

    // Calculate alpha, beta, gamma
    float alpha, beta, gamma;
    float ra = cylinder.getRadiusDerivativeWRTa();
//    qDebug() << "ra: " << ra;
    QMatrix4x4 m(QVector3D::dotProduct(sa,sa), QVector3D::dotProduct(st,sa),0,0,
                              QVector3D::dotProduct(sa,st), QVector3D::dotProduct(st,st),0,0,
                                0,0,1,0,0,0,0,1);
//    qDebug() << "m" << m;
    QMatrix4x4 mInv = m.inverted();
//    qDebug() << "mInv: " << mInv;
    QVector4D b = QVector4D(-ra, 0,0,0);
    QVector4D x = mInv*b;
    alpha = x.x();
    beta = x.y();
//    qDebug() << "A11"<< mInv.column(0).x();
    gamma = sqrt(1-ra*ra*(mInv.column(0).x()));
//    qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma;

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    // qDebug() << "norm: " << normal;
    normal.normalize();
    return normal;
}
    
