#include "simplepath.h"

/**
 * @brief SimplePath::SimplePath Default constructor for SimplePath.
*/
SimplePath::SimplePath()
    : x(Polynomial()), y(Polynomial()), z(Polynomial())
{
    t0 = 0;
    t1 = 5;
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
    t1 = 5;
}

/**
 * @brief SimplePath::getPathAt Returns the path at a given time.
 * @param t
 * @return
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
 * @param time
 * @return
*/
int SimplePath::getIdxAtTime(float time)
{
    float delta = (t1-t0)/sectors;
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
