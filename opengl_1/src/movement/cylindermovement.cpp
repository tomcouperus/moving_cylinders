#include "cylindermovement.h"
#include "mathutility.h"

/**
 * @brief CylinderMovement::CylinderMovement Default constructor, sets the path with default path
 * constructor and defines the directional vector of the movement to always be (0,1,0)
 */
CylinderMovement::CylinderMovement() {}

CylinderMovement::CylinderMovement(SimplePath path) :
    path(path)
{}


CylinderMovement::CylinderMovement(SimplePath path, const Tool *tool) :
    path(path),
    toolAxis(tool->getAxisVector())
{}

/**
 * @brief CylinderMovement::CylinderMovement Constructor
 * @param path Path that describes the cylinder (bottom) location through time
 * @param axisDirection1 initial directional vector (direction of the cylinder axis at time 0)
 * @param axisDirection2 final directional vector (direction of the cylinder axis at time 1)
 * @param cylinder cylinder which experiences the movement
 * (this class can be generalized and constructors for other tools can be made)
 */
CylinderMovement::CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, const Tool *tool) :
    path(path),
    axisT0(axisDirection1),
    axisT1(axisDirection2),
    toolAxis(tool->getAxisVector())
{}


/**
 * @brief CylinderMovement::setAxisDirections Sets the axis directions of the cylinder through time
 * @param axisDirection1 initial directional vector (direction of the cylinder axis at time 0)
 * @param axisDirection2 final directional vector (direction of the cylinder axis at time 1)
 * @return true if the vectors are valid, false otherwise
 */
bool CylinderMovement::setAxisDirections(QVector3D axisDirection1, QVector3D axisDirection2){

    if(axisDirection1 == QVector3D(0,0,0) || axisDirection2 == QVector3D(0,0,0)){
        return false;
    }

    axisT0 = axisDirection1;
    axisT1 = axisDirection2;

    return true;
}

/**
 * @brief CylinderMovement::getAxisAt returns the axis direction at time time
 * @param time time of interest
 * @return axis direction
 */
QVector3D CylinderMovement::getAxisAt(float time)
{
    QVector3D axis = axisT0 + (axisT1 - axisT0)*time;
    axis.normalize();
    return axis;
}

/**
 * @brief CylinderMovement::getAxisAt returns the derivative of the axis direction at time time
 * @param time time of interest
 * @return axis direction
 */
QVector3D CylinderMovement::getAxisDtAt(float time)
{
    QVector3D axis = axisT0 + (axisT1 - axisT0)*time;
    QVector3D axis_t = axisT1 - axisT0;
    return MathUtility::normalVectorDerivative(axis, axis_t);
}

/**
 * @brief CylinderMovement::getAxisDt2At returns the second derivative of the axis direction at time time
 * @param time time of interest
 * @return axis direction
 */
QVector3D CylinderMovement::getAxisDt2At(float time)
{
    QVector3D axis = axisT0 + (axisT1 - axisT0)*time;
    QVector3D axis_t = axisT1 - axisT0;
    QVector3D axis_tt(0,0,0);
    return MathUtility::normalVectorDerivative2(axis, axis_t, axis_tt);
}

/**
 * @brief CylinderMovement::getAxisDt3At returns the third derivative of the axis direction at time time
 * @param time time of interest
 * @return axis direction
 */
QVector3D CylinderMovement::getAxisDt3At(float time)
{
    QVector3D axis = axisT0 + (axisT1 - axisT0)*time;
    QVector3D axis_t = axisT1 - axisT0;
    QVector3D axis_tt(0,0,0);
    QVector3D axis_ttt(0,0,0);
    return MathUtility::normalVectorDerivative3(axis, axis_t, axis_tt, axis_ttt);
}

/**
 * @brief CylinderMovement::getRotationVectorAt returns the vector that the tool should be rotated around at time
 * @param time time of interest
 * @return rotation vector
 */
QVector3D CylinderMovement::getRotationVectorAt(float time)
{
    return QVector3D::crossProduct(toolAxis, getAxisAt(time));
}
