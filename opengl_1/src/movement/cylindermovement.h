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

public:
    CylinderMovement();
    CylinderMovement(SimplePath path);
    CylinderMovement(SimplePath path, const Tool *tool);
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, const Tool *tool);

    inline SimplePath& getPath() {return path;}
    bool setAxisDirections(QVector3D axisDirection1, QVector3D axisDirection2);
    inline QVector<Vertex>& getPathVertexArr() { return path.getVertexArr(); }

    inline QVector3D getAxisT0() const { return axisT0; }
    inline QVector3D getAxisT1() const { return axisT1; }

    QVector3D getAxisAt(float time);
    QVector3D getAxisDtAt(float time);
    QVector3D getAxisDt2At(float time);
    QVector3D getAxisDt3At(float time);

    QVector3D getRotationVectorAt(float time);
};

#endif // CYLINDERMOVEMENT_H
