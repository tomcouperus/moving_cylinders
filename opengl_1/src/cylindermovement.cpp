#include "cylindermovement.h"

CylinderMovement::CylinderMovement()
{
    path = SimplePath();
    QVector<Vertex> vertices = path.getVertexArr();
    for (size_t i = 0; i < vertices.size(); i++)
    {
        axisDirections.append(QVector3D(0, 1, 0));
    }
}

CylinderMovement::CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Cylinder cylinder) :
    path(path), cylinder(cylinder)
{
    QVector<Vertex> vertices = path.getVertexArr();
    QVector3D lastDirection = axisDirection1;
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (vertices.size()-1);
    qDebug() << deltaDirection;
    for (size_t i = 0; i < vertices.size(); i++)
    {
        qDebug() << lastDirection;
        axisDirections.append(lastDirection);
        QVector3D rotationVector = QVector3D::crossProduct(cylinder.getAxisVector(),
                                                           lastDirection);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // rotation vector if vectors are anti-parallel (there are infinitely many)
            rotationVector = cylinder.getVectorPerpToAxis(); // take the one saved on the cylinder info
        }
        rotationVectors.append(rotationVector);
        lastDirection += deltaDirection;
    }
}

CylinderMovement::CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections, Cylinder cylinder) :
    path(path),
    axisDirections(axisDirections),
    cylinder(cylinder)
{}

QMatrix4x4 CylinderMovement::getMovementRotation(int idx)
{
    QMatrix4x4 cylinderRotation;
    QVector3D initVector = axisDirections[idx];
    float angle = acos(QVector3D::dotProduct(initVector.normalized(), cylinder.getAxisVector()));
    angle = qRadiansToDegrees(angle);
    qDebug() << angle;
    if(angle != 0) {
        cylinderRotation.rotate(angle, rotationVectors[idx]);
    }
    return cylinderRotation;
}

void CylinderMovement::rotateAxisDirections(QMatrix4x4 rotation)
{
    for (size_t i = 0; i < axisDirections.size(); i++){
        axisDirections[i] = rotation.map(axisDirections[i]);
        rotationVectors[i] = rotation.map(rotationVectors[i]);
    }
}
