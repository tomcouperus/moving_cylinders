#include "cubicpath.h"

CubicPath::CubicPath()
    : a(1.0), b(0.0), c(0.0), d(0.0)
{

}
CubicPath::CubicPath(float a, float b, float c, float d)
    : a(a), b(b), c(c), d(d)
{

}

QVector3D CubicPath::getPathAt(float t)
{
    QVector3D pt;
    return pt;
}
