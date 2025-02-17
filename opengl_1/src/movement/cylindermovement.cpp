#include "cylindermovement.h"

/**
 * @brief CylinderMovement::CylinderMovement Default constructor, sets the path with default path
 * constructor and defines the directional vector of the movement to always be (0,1,0)
 */
CylinderMovement::CylinderMovement() :
    path(SimplePath()),
    cylinderAxis(Cylinder().getAxisVector()),
    perpToAxis(Cylinder().getVectorPerpToAxis()),
    discPath(path.getVertexArr())
{
    t0 = path.getT0(); t1 = path.getT1();
    QVector<Vertex> vertices = path.getVertexArr();
    QVector3D rotationVector = QVector3D::crossProduct(cylinderAxis, QVector3D(0, 1, 0));
    for (size_t i = 0; i < vertices.size(); i++)
    {
        axisDirections.append(QVector3D(0, 1, 0));
        rotationVectors.append(rotationVector);
    }
}

/**
 * @brief CylinderMovement::CylinderMovement Constructor
 * @param path Path that describes the cylinder (bottom) location through time
 * @param axisDirection1 initial directional vector (direction of the cylinder axis at time 0)
 * @param axisDirection2 final directional vector (direction of the cylinder axis at time 1)
 * @param cylinder cylinder which experiences the movement
 * (this class can be generalized and constructors for other tools can be made)
 */
CylinderMovement::CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Cylinder cylinder) :
    path(path),
    cylinderAxis(cylinder.getAxisVector()),
    perpToAxis(cylinder.getVectorPerpToAxis()),
    discPath(path.getVertexArr())
{
    t0 = path.getT0(); t1 = path.getT1();

    // interpolation of directional vectors
    QVector3D lastDirection = axisDirection1;
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (path.getSectors());
    for (size_t i = 0; i <= path.getSectors(); i++)
    {
        axisDirections.append(lastDirection.normalized()); // add interpolated directional vector

        // Get (and save) vector perpendicular to both the directional vector of the movement and the axis of the cylinder
        QVector3D rotationVector = QVector3D::crossProduct(cylinderAxis,
                                                           lastDirection);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // rotation vector if vectors are anti-parallel
                                                       // (there are infinitely many)
            rotationVector = perpToAxis; // take the one saved on the cylinder info
        }
        rotationVectors.append(rotationVector.normalized());

        lastDirection += deltaDirection;
    }
}

/**
 * @brief CylinderMovement::CylinderMovement Constructor
 * @param path Path that describes the cylinder (bottom) location through time
 * @param axisDirection1 initial directional vector (direction of the cylinder axis at time 0)
 * @param axisDirection2 final directional vector (direction of the cylinder axis at time 1)
 * @param drum drum which experiences the movement
 */
CylinderMovement::CylinderMovement(SimplePath path, QVector3D axisDirection1, QVector3D axisDirection2, Drum drum) :
    path(path),
    cylinderAxis(drum.getAxisVector()),
    perpToAxis(drum.getVectorPerpToAxis()),
    discPath(path.getVertexArr())
{
    t0 = path.getT0(); t1 = path.getT1();

    // interpolation of directional vectors
    QVector3D lastDirection = axisDirection1;
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (path.getSectors());
    for (size_t i = 0; i <= path.getSectors(); i++)
    {
        axisDirections.append(lastDirection.normalized()); // add interpolated directional vector

        // Get (and save) vector perpendicular to both the directional vector of the movement and the axis of the cylinder
        QVector3D rotationVector = QVector3D::crossProduct(cylinderAxis,
                                                           lastDirection);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // rotation vector if vectors are anti-parallel
            // (there are infinitely many)
            rotationVector = perpToAxis; // take the one saved on the cylinder info
        }
        rotationVectors.append(rotationVector);

        lastDirection += deltaDirection;
    }
}

/**
 * @brief CylinderMovement::CylinderMovement
 * @param path Path that describes the cylinder (bottom) location through time
 * @param axisDirections directional vectors (directions of the cylinder axis through time)
 * @param cylinder
 */
CylinderMovement::CylinderMovement(SimplePath path, QVector<QVector3D> axisDirections, Cylinder cylinder) :
    path(path),
    axisDirections(axisDirections),
    cylinderAxis(cylinder.getAxisVector()),
    perpToAxis(cylinder.getVectorPerpToAxis()),
    discPath(path.getVertexArr())
{
    t0 = path.getT0(); t1 = path.getT1();

    for (size_t i = 0; i < path.getSectors(); i++)
    {
        QVector3D rotationVector = QVector3D::crossProduct(cylinderAxis,
                                                           axisDirections[i]);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // rotation vector if vectors are anti-parallel (there are infinitely many)
            rotationVector = perpToAxis; // take the one saved on the cylinder info
        }
        rotationVectors.append(rotationVector);
    }
}

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

    axisDirections.clear();
    rotationVectors.clear();

    // interpolation of directional vectors
    QVector3D lastDirection = axisDirection1;
    QVector3D deltaDirection = (axisDirection2 - axisDirection1) / (path.getSectors());
    for (size_t i = 0; i <= path.getSectors(); i++)
    {
        axisDirections.append(lastDirection.normalized()); // add interpolated directional vector

        // Get (and save) vector perpendicular to both the directional vector of the movement and the axis of the cylinder
        QVector3D rotationVector = QVector3D::crossProduct(cylinderAxis,
                                                           lastDirection);
        if(rotationVector == QVector3D(0.0,0.0,0.0)) { // rotation vector if vectors are anti-parallel
            // (there are infinitely many)
            rotationVector = perpToAxis; // take the one saved on the cylinder info
        }
        rotationVectors.append(rotationVector);

        lastDirection += deltaDirection;
    }

    return true;
}

/**
 * @brief CylinderMovement::getMovementRotation Calculates the rotation that the cylinder needs to undergo
 * to be positioned with its axis in the direction described by the movement at time time
 * @param time time of interest
 * @return Matrix describing the rotation
 */
QMatrix4x4 CylinderMovement::getMovementRotation(float time)
{
    QMatrix4x4 cylinderRotation;
    cylinderRotation.setToIdentity();
    QVector3D initVector = getAxisDirectionAt(time);
    float angle = acos(QVector3D::dotProduct(initVector.normalized(), cylinderAxis.normalized()));
    angle = qRadiansToDegrees(angle);
    if(angle != 0) {
        cylinderRotation.rotate(angle, getRotationVectorAt(time));
    }
    return cylinderRotation;
}

/**
 * @brief CylinderMovement::getAxisRateOfChange Calculates the rate of change of the axis of the cylinder
 * @param time time of interest
 * @return rate of change of the axis
 */
QVector3D CylinderMovement::getAxisRateOfChange(float time)
{
    int idx = path.getIdxAtTime(time);
    QVector3D axisRateOfChange;
    QVector3D nextAxis;
    if(idx == axisDirections.size()-1) {
        nextAxis = axisDirections[idx-1];
        axisRateOfChange = (axisDirections[idx] - nextAxis);
    } else {
        nextAxis = axisDirections[idx+1];
        axisRateOfChange = (nextAxis - axisDirections[idx]);
    }
    return axisRateOfChange;
}

/**
 * @brief CylinderMovement::rotateAxisDirections rotates the axis directions uniformly
 * @param rotation matrix describing the rotation
 */
void CylinderMovement::rotateAxisDirections(QMatrix4x4 rotation)
{
    for (size_t i = 0; i < axisDirections.size(); i++){
        axisDirections[i] = rotation.map(axisDirections[i]);
        rotationVectors[i] = rotation.map(rotationVectors[i]);
    }
}

/**
 * @brief CylinderMovement::getAxisDirectionAt returns the axis direction at time time
 * @param time time of interest
 * @return axis direction
 */
QVector3D CylinderMovement::getAxisDirectionAt(float time)
{
    int idx = path.getIdxAtTime(time);
    return axisDirections[idx];
}

/**
 * @brief CylinderMovement::getRotationVectorAt returns the rotation vector at time time
 * @param time time of interest
 * @return rotation vector
 */
QVector3D CylinderMovement::getRotationVectorAt(float time)
{
    int idx = path.getIdxAtTime(time);
    return rotationVectors[idx];
}
