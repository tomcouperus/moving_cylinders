#ifndef CYLINDERMOVEMENT_H
#define CYLINDERMOVEMENT_H

#include "simplepath.h"
#include <QVector>
#include <QVector3D>

class CylinderMovement
{
    SimplePath path;
    QVector<QVector3D> axisDirections;
public:
    CylinderMovement();
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2);
    CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections);
    inline void setPath(SimplePath path) {this->path = path;}
    inline void getAxisDirections(QVector<QVector3D> axisDirections) {this->axisDirections = axisDirections;}
};

#endif // CYLINDERMOVEMENT_H
