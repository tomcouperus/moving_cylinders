#ifndef CUBICPATH_H
#define CUBICPATH_H

#include "path.h"

class CubicPath //: public Path
{
    // P(t) = at³ + bt² * ct + d
    float a, b, c, d;
public:
    CubicPath();
    CubicPath(float a, float b, float c, float d);

    QVector3D getPathAt(float t);
};

#endif // CUBICPATH_H
