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

CylinderMovement::CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2) :
    path(path)
{
    QVector<Vertex> vertices = path.getVertexArr();
    axisDirections.append(axisDirection1);
    QVector3D lastDirection = axisDirection1;
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (vertices.size() - 1);
    qDebug() << deltaDirection;
    for (size_t i = 1; i < vertices.size()-1; i++)
    {
        lastDirection += deltaDirection;
        qDebug() << lastDirection;
        axisDirections.append(lastDirection);
    }
    axisDirections.append(axisDirection2);
}

CylinderMovement::CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections) :
    path(path),
    axisDirections(axisDirections)
{}

QMatrix4x4 CylinderMovement::getMovementRotation(int idx, Cylinder cylinder)
{
    QMatrix4x4 cylinderRotation;
    QVector3D initVector = axisDirections[idx];
    float angle = acos(QVector3D::dotProduct(initVector.normalized(), cylinder.getAxisVector()));
    angle = qRadiansToDegrees(angle);
    qDebug() << angle;
    if(angle != 0) {
        QVector3D rotationVector = QVector3D::crossProduct(cylinder.getAxisVector(),
                                                           initVector);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // cheaty fix for finding the rotation vector if vectors are anti-parallel
            rotationVector = cylinder.getVectorPerpToAxis();
        }
        cylinderRotation.rotate(angle, rotationVector);
    }
    return cylinderRotation;
}
