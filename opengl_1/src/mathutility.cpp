#include "mathutility.h"

MathUtility::MathUtility() {}

/**
 * @brief normalVectorDerivative Calculates the derivative of a normalized vector.
 * @param a The unnormalized vector
 * @param da The derivative of the unnormalized vector
 * @return
 */
QVector3D MathUtility::normalVectorDerivative(QVector3D a, QVector3D da)
{
    // Unit vector b
    float L = a.length();
    // QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    QVector3D db = c / L2;
    return db;
}

/**
 * @brief normalVectorDerivative2 Calculates the second derivative of a normalized vector.
 * @param a The unnormalized vector
 * @param da The derivative of the unnormalized vector
 * @param dda The second derivative of the unnormalized vector
 * @return
 */
QVector3D MathUtility::normalVectorDerivative2(QVector3D a, QVector3D da, QVector3D dda)
{
    // Unit vector b
    float L = a.length();
    // QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    // QVector3D db = c / L2;

    // Second derivative ddb
    float m = L * (QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) - QVector3D::dotProduct(a, da) * dL;
    float ddL = m / L2;
    QVector3D dc = L * dda - a * ddL;
    float L4 = L2 * L2;
    float dL2 = 2 * L * dL;
    QVector3D f = L2 * dc - c * dL2;
    QVector3D ddb = f / L4;
    return ddb;
}

/**
 * @brief normalVectorDerivative3 Calculates the third derivative of a normalized vector.
 * @param a The unnormalized vector
 * @param da The derivative of the unnormalized vector
 * @param dda The second derivative of the unnormalized vector
 * @param ddda The third derivative of the unnormalized vector
 * @return
 */
QVector3D MathUtility::normalVectorDerivative3(QVector3D a, QVector3D da, QVector3D dda, QVector3D ddda)
{
    // Unit vector b
    float L = a.length();
    // QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    // QVector3D db = c / L2;

    // Second derivative ddb
    float m = L * (QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) - QVector3D::dotProduct(a, da) * dL;
    float ddL = m / L2;
    QVector3D dc = L * dda - a * ddL;
    float L4 = L2 * L2;
    float dL2 = 2 * L * dL;
    QVector3D f = L2 * dc - c * dL2;
    // QVector3D ddb = f / L4;

    // Third derivative dddb
    float dm = L * (QVector3D::dotProduct(a, ddda) + 3 * QVector3D::dotProduct(da, dda)) - QVector3D::dotProduct(a, da) * ddL;
    float dddL = (L2 * dm - m * dL2) / L4;
    QVector3D ddc = (dL * dda + L * ddda) - (da * ddL + a * dddL);
    float ddL2 = 2 * (dL * dL + L * ddL);
    QVector3D df = L2 * ddc - c * ddL2;
    float L8 = L4 * L4;
    float dL4 = 4 * (L * L * L) * dL;
    QVector3D dddb = (L4 * df - f * dL4) / L8;

    return dddb;
}
