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
    float t0;
    float t1;

    QVector3D cylinderAxis; // or tool axis
    QVector3D perpToAxis;
    QVector<QVector3D> axisDirections;
    QVector<QVector3D> rotationVectors;
    QVector<Vertex> discPath;
public:
    CylinderMovement();
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Cylinder cylinder);
    CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Drum drum);
    CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections, Cylinder cylinder);

    inline QVector<Vertex> getPathVertexArr() { return this->discPath; }
    inline void setDiscPath(QVector<Vertex> discPath) { this->discPath.clear(); this->discPath = discPath;
        qDebug() << "path size:" << this->discPath.size();}
    inline void setPath(SimplePath path) { t0 = path.getT0(); t1 = path.getT1(); this->path = path; discPath = path.getVertexArr(); }
    inline SimplePath getPath() {return path;}
    inline void setAxisDirections(QVector<QVector3D> axisDirections) {axisDirections.clear(); this->axisDirections = axisDirections;}
    bool setAxisDirections(QVector3D axisDirection1, QVector3D axisDirection2);
    inline QVector<QVector3D> getAxisDirections() {return axisDirections;}

    QMatrix4x4 getMovementRotation(float time);
    QVector3D getAxisRateOfChange(float time);
    void rotateAxisDirections(QMatrix4x4 rotation);
    QVector3D getAxisDirectionAt(float time);
    QVector3D getRotationVectorAt(float time);
};

#endif // CYLINDERMOVEMENT_H
