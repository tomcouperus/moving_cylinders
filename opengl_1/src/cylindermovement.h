#ifndef CYLINDERMOVEMENT_H
#define CYLINDERMOVEMENT_H

#include "simplepath.h"
#include "cylinder.h"
#include <QVector>
#include <QVector3D>
#include <QOpenGLDebugLogger>
#include <QMatrix4x4>

class CylinderMovement
{
    SimplePath path;
    QVector<QVector3D> axisDirections;
public:
    CylinderMovement();
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2);
    CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections);
    inline void setPath(SimplePath path) {this->path = path;}
    inline void setAxisDirections(QVector<QVector3D> axisDirections) {this->axisDirections = axisDirections;}
    inline QVector<QVector3D> getAxisDirections() {return axisDirections;}

    QMatrix4x4 getMovementRotation(int idx, Cylinder cylinder);
};

#endif // CYLINDERMOVEMENT_H
