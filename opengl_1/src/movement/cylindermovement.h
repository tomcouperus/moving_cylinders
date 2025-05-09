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

    QVector3D toolAxis; // Internal axis of tool model. Used for rotating tool into proper place

    QVector3D axisT0{0.0f, 1.0f, 0.0f};
    QVector3D axisT1{0.0f, 1.0f, 0.0f};

    QVector<Vertex> discPath; // Path of tip of tool
public:
    CylinderMovement();
    CylinderMovement(SimplePath path);
    CylinderMovement(SimplePath path, const Tool *tool);
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, const Tool *tool);

    inline SimplePath& getPath() {return path;}
    bool setAxisDirections(QVector3D axisDirection1, QVector3D axisDirection2);
    inline QVector<Vertex>& getPathVertexArr() { return this->discPath; }

    QVector3D getAxisAt(float time);
    QVector3D getAxisDtAt(float time);
    QVector3D getAxisDt2At(float time);

    QMatrix4x4 getMovementRotation(float time);
    QVector3D getRotationVectorAt(float time);

    inline void setToolAxis(const Tool *tool) { toolAxis = tool->getAxisVector(); }
};

#endif // CYLINDERMOVEMENT_H
