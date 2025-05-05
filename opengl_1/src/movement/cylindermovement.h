#ifndef CYLINDERMOVEMENT_H
#define CYLINDERMOVEMENT_H

#include "simplepath.h"
#include "../tools/cylinder.h"
#include "../tools/drum.h"
#include <QVector>
#include <QVector3D>
#include <QOpenGLDebugLogger>
#include <QMatrix4x4>

class CylinderMovement
{
    SimplePath path;

    QVector3D cylinderAxis; // or tool axis
    QVector3D perpToAxis;

    QVector<Vertex> discPath; // Path of tip of tool
public:
    CylinderMovement();
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Cylinder cylinder);
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Drum drum);
    CylinderMovement(SimplePath path, Cylinder cylinder);

    inline QVector<Vertex>& getPathVertexArr() { return this->discPath; }
    inline SimplePath& getPath() {return path;}
    bool setAxisDirections(QVector3D axisDirection1, QVector3D axisDirection2);

    QMatrix4x4 getMovementRotation(float time);
    QVector3D getAxisRateOfChange(float time);
    void rotateAxisDirections(QMatrix4x4 rotation);
    QVector3D getAxisDirectionAt(float time);
    QVector3D getRotationVectorAt(float time);
};

#endif // CYLINDERMOVEMENT_H
