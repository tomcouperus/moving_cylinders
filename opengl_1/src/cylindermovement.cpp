#include "cylindermovement.h"

CylinderMovement::CylinderMovement()
{
    path = SimplePath();
    QVector<Vertex> vertices = path.getVertexArr();
    for (size_t i = 0; i < sizeof(vertices); i++)
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
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (sizeof(vertices) - 1);
    for (size_t i = 1; i < sizeof(vertices); i++)
    {
        lastDirection += deltaDirection;
        axisDirections.append(lastDirection);
    }
}

CylinderMovement::CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections) :
    path(path),
    axisDirections(axisDirections)
{}
