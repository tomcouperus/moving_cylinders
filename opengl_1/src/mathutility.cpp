#include "mathutility.h"

MathUtility::MathUtility() {}


QVector3D MathUtility::normalVectorDerivative(QVector3D a, QVector3D da) {
    float l = a.length();
    float dl = QVector3D::dotProduct(a, da) / l;
    return (l * da - a * dl) / (l * l);
}
QVector3D MathUtility::normalVectorDerivative2(QVector3D a, QVector3D da, QVector3D dda) {
    float l = a.length();
    float dl = QVector3D::dotProduct(a, da) / l;
    float ddl = (l * (QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) - QVector3D::dotProduct(a, da) * dl) / (l * l);

    QVector3D c = l * da - a * dl;
    QVector3D dc = l * dda - a * ddl;
    return (l * l * dc - c * 2 * l * dl) / (l * l * l * l);
}
