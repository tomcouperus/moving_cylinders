#include "simplepath.h"

SimplePath::SimplePath()
    : x(Polynomial()), y(Polynomial()), z(Polynomial())
{
    t0 = 0;
    t1 = 5;
}

SimplePath::SimplePath(Polynomial x, Polynomial y, Polynomial z)
    : x(x), y(y), z(z)
{
    t0 = 0;
    t1 = 5;
}

QVector3D SimplePath::getPathAt(float t)
{
    QVector3D pt = QVector3D();
    pt.setX(x.getValAt(t));
    pt.setY(y.getValAt(t));
    pt.setZ(z.getValAt(t));
    return pt;
}

int SimplePath::getIdxAtTime(float time)
{
    float delta = (t1-t0)/sectors;
    return (time-t0)/delta;
}

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
