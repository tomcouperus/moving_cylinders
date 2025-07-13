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
    QVector3D b = a / L;

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
    QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    QVector3D db = c / L2;

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
    QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    QVector3D db = c / L2;

    // Second derivative ddb
    float m = L * (QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) - QVector3D::dotProduct(a, da) * dL;
    float ddL = m / L2;
    QVector3D dc = L * dda - a * ddL;
    float L4 = L2 * L2;
    float dL2 = 2 * L * dL;
    QVector3D f = L2 * dc - c * dL2;
    QVector3D ddb = f / L4;

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

/**
 * @brief normalVectorDerivative4 Calculates the fourth derivative of a normalized vector.
 * @param a The unnormalized vector
 * @param da The derivative of the unnormalized vector
 * @param dda The second derivative of the unnormalized vector
 * @param ddda The third derivative of the unnormalized vector
 * @param dddda The fourth derivative of the unnormalized vector
 * @return
 */
QVector3D MathUtility::normalVectorDerivative4(QVector3D a, QVector3D da, QVector3D dda, QVector3D ddda, QVector3D dddda)
{
    // Unit vector b
    float L = a.length();
    QVector3D b = a / L;

    // First derivative db
    float dL = QVector3D::dotProduct(a, da) / L;
    QVector3D c = L * da - a * dL;
    float L2 = L * L;
    QVector3D db = c / L2;

    // Second derivative ddb
    float m = L * (QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) - QVector3D::dotProduct(a, da) * dL;
    float ddL = m / L2;
    QVector3D dc = L * dda - a * ddL;
    float L4 = L2 * L2;
    float dL2 = 2 * L * dL;
    QVector3D f = L2 * dc - c * dL2;
    QVector3D ddb = f / L4;

    // Third derivative dddb
    float dm = L * (QVector3D::dotProduct(a, ddda) + 3 * QVector3D::dotProduct(da, dda)) - (QVector3D::dotProduct(a, da) * ddL);
    float n = L2 * dm - m * dL2;
    float dddL = n / L4;
    QVector3D ddc = (dL * dda + L * ddda) - (da * ddL + a * dddL);
    float ddL2 = 2 * (dL * dL + L * ddL);
    QVector3D df = L2 * ddc - c * ddL2;
    float L8 = L4 * L4;
    float dL4 = 4 * pow(L, 3) * dL;
    QVector3D g = L4 * df - f * dL4;
    QVector3D dddb = g / L8;

    // Fourth derivative ddddb
    float ddm = (dL * (QVector3D::dotProduct(a, ddda) + 3 * QVector3D::dotProduct(da, dda)) + L * (QVector3D::dotProduct(a, dddda) + 3 * QVector3D::dotProduct(dda, dda) + 4 * QVector3D::dotProduct(da, ddda))) - ((QVector3D::dotProduct(da, da) + QVector3D::dotProduct(a, dda)) * ddL + QVector3D::dotProduct(a, da) * dddL);
    float dn = L2 * ddm - m * ddL2;
    float ddddL = L4 * dn - n * dL4 / L8;
    QVector3D dddc = (ddL * dda + dL * ddda + dL * ddda + L * dddda) - (dda * ddL + da * dddL + da * dddL + a * ddddL);
    float dddL2 = 2 * (2 * dL * ddL + dL * ddL + L * dddL);
    QVector3D ddf = (dL2 * ddc + L2 * dddc) - (dc * ddL2 + c * dddL2);
    float ddL4 = 4 * (3 * pow(L, 2) * dL + pow(L, 3) * ddL);
    QVector3D dg = L4 * ddf - f * ddL4;
    float L16 = L8 * L8;
    float dL8 = 8 * pow(L, 7) * dL;
    QVector3D h = L8 * dg - g * dL8;
    QVector3D ddddb = h / L16;

    return ddddb;
}
