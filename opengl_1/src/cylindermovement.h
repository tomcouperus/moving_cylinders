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
    QVector3D cylinderAxis; // or tool axis
    QVector3D perpToAxis;
    QVector<QVector3D> axisDirections;
    QVector<QVector3D> rotationVectors;
public:
    CylinderMovement();
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Cylinder cylinder);
    CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections, Cylinder cylinder);
    inline void setPath(SimplePath path) {this->path = path;}
    inline SimplePath getPath() {return path;}
    inline void setAxisDirections(QVector<QVector3D> axisDirections) {this->axisDirections = axisDirections;}
    inline QVector<QVector3D> getAxisDirections() {return axisDirections;}

    QMatrix4x4 getMovementRotation(float time);
    void rotateAxisDirections(QMatrix4x4 rotation);
};

#endif // CYLINDERMOVEMENT_H
