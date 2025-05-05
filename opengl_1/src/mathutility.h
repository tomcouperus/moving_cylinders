#ifndef MATHUTILITY_H
#define MATHUTILITY_H

#include <QVector3D>

class MathUtility
{
public:
    MathUtility();

    static QVector3D normalVectorDerivative(QVector3D a, QVector3D da);
    static QVector3D normalVectorDerivative2(QVector3D a, QVector3D da, QVector3D dda);
};

#endif // MATHUTILITY_H
