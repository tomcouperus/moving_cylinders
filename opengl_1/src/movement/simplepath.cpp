#include "simplepath.h"

/**
 * @brief SimplePath::SimplePath Default constructor for SimplePath.
*/
SimplePath::SimplePath()
    : x(Polynomial()), y(Polynomial()), z(Polynomial())
{}

/**
 * @brief SimplePath::SimplePath Constructor for SimplePath.
 * @param x Polynomial for x component.
 * @param y Polynomial for y component.
 * @param z Polynomial for z component.
*/
SimplePath::SimplePath(Polynomial x, Polynomial y, Polynomial z)
    : x(x), y(y), z(z)
{}

/**
 * @brief SimplePath::getPathAt Returns the path at a given time.
 * @param t Time.
 * @return Path at time t.
*/
QVector3D SimplePath::getPathAt(float t)
{
    QVector3D pt = QVector3D();
    pt.setX(x.getValAt(t));
    pt.setY(y.getValAt(t));
    pt.setZ(z.getValAt(t));
    return pt;
}

/**
 * @brief SimplePath::updateVertexArr Updates the vertex array.
*/
void SimplePath::updateVertexArr()
{
    vertexArr.clear();
    QVector3D color(0,0,1);

    for(int i=0; i<=sectors; ++i){
        float t = (float) i / sectors;
        vertexArr.append(Vertex(getPathAt(t), color));
    }
}

/**
 * @brief SimplePath::getTangentAt Returns the tangent at a given time.
 * @param t Time.
 * @return Tangent at time t.
*/
QVector3D SimplePath::getTangentAt(float t)
{
    QVector3D tangent = QVector3D();
    tangent.setX(x.getDerivativeAt(t));
    tangent.setY(y.getDerivativeAt(t));
    tangent.setZ(z.getDerivativeAt(t));
    return tangent;
}

QVector3D SimplePath::getAccelerationAt(float t) {
    QVector3D acc = QVector3D();
    acc.setX(x.getDerivative2At(t));
    acc.setY(y.getDerivative2At(t));
    acc.setZ(z.getDerivative2At(t));
    return acc;
}


