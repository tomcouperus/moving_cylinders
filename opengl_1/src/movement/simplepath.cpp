#include "simplepath.h"

/**
 * @brief SimplePath::SimplePath Default constructor for SimplePath.
*/
SimplePath::SimplePath()
    : x(Polynomial()), y(Polynomial()), z(Polynomial())
{
    t0 = 0;
    t1 = 2;
}

/**
 * @brief SimplePath::SimplePath Constructor for SimplePath.
 * @param x Polynomial for x component.
 * @param y Polynomial for y component.
 * @param z Polynomial for z component.
*/
SimplePath::SimplePath(Polynomial x, Polynomial y, Polynomial z)
    : x(x), y(y), z(z)
{
    t0 = 0;
    t1 = 2;
}

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
 * @brief SimplePath::getIdxAtTime Returns the index of the vertex array at a given time.
 * @param time Time.
 * @return Index of the vertex array at time.
*/
int SimplePath::getIdxAtTime(float time)
{
    float delta = (t1-t0)/(sectors+1);
    if ((time-t0)/delta > vertexArr.size()-1)
        return vertexArr.size()-1;
    return (time-t0)/delta;
}

/**
 * @brief SimplePath::initVertexArr Initializes the vertex array.
*/
void SimplePath::initVertexArr()
{
    vertexArr.clear();
    QVector3D color(0,0,1);
    float delta = (t1-t0)/sectors;
    float t = t0;
    for(int i=0; i<=sectors; ++i){
        vertexArr.append(Vertex(getPathAt(t), color));
        qDebug() << t;
        t += delta;
    }
}

/**
 * @brief SimplePath::updateVertexArr Updates the vertex array.
*/
void SimplePath::updateVertexArr()
{
    vertexArr.clear();
    QVector3D color(0,0,1);
    float delta = (t1-t0)/sectors;
    float t = t0;
    for(int i=0; i<=sectors; ++i){
        vertexArr.append(Vertex(getPathAt(t), color));
        t += delta;
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


