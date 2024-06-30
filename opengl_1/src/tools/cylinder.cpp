#include "cylinder.h"

/**
 * @brief Cylinder::Cylinder Default constructor builds cylinder to preset size
 */
Cylinder::Cylinder()
    : r(0.5), angle(0.0)

{
    this->a0 = r*tan(angle);
    float r1 = r + height*tan(angle);
    this->a1 = height + r1*tan(angle);
}

/**
 * @brief Cylinder::Cylinder Cylinder constructor
 * @param baseRadius radius of the smaller end
 * @param angle opening angle
 * @param height cylinder height
 * @param sectors number of horizontal and vertical sectors for the cylinder
 * @param position position of the center of the base of the cylinder
 */
Cylinder::Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position)
    : r(baseRadius), angle(angle)
{
    this->sectors = sectors;
    this->posit = position;
    this->a0 = r*tan(angle);
    float r1 = r + height*tan(angle);
    this->a1 = height + r1*tan(angle);
}

/**
 * @brief Cylinder::initCylinder Initializes the vertex array for the cylinder.
 */
void Cylinder::initCylinder(){
    computeCylinder();
}

void Cylinder::setSectors(int sectors){
    this->sectors = sectors;
    computeCylinder();
}

void Cylinder::setRadius(float radius){
    r = radius;
    this->a0 = r*tan(angle);
    float r1 = r + height*tan(angle);
    this->a1 = height + r1*tan(angle);
    computeCylinder();
}

void Cylinder::setAngle(float angle){
    this->angle = angle;
    this->a0 = r*tan(angle);
    float r1 = r + height*tan(angle);
    this->a1 = height + r1*tan(angle);
    computeCylinder();
}

void Cylinder::setHeight(float height){
    this->height = height;
    float r1 = r + height*tan(angle);
    this->a1 = height + r1*tan(angle);
    computeCylinder();
}

void Cylinder::setPosit(QVector3D position){
    posit = position;
    computeCylinder();
}

/**
 * @brief Cylinder::computeCylinder computes the vertex array for the cylinder.
 */
void Cylinder::computeCylinder(){
    vertexArr.clear();
    Vertex v1, v2, v3, v4;
    float hDelta = height/sectors;
    float r0, r1 = r;
    float h0, h1 = 0;
    float theta = 2*PI/sectors;
    for(size_t j=0;j<sectors;j++){
        // transition to next layer
        h0 = h1;
        h1 += hDelta;
        r0 = r1;
        r1 = r + h1*tan(angle);
        for(size_t i=0;i<sectors;i++){
            // Calculate vertices of a quad of the cylinder
            v1 = calcCircleBound(r0, theta*i, h0);
            v2 = calcCircleBound(r1, theta*i, h1);
            v3 = calcCircleBound(r0, theta*(i+1), h0);
            v4 = calcCircleBound(r1, theta*(i+1), h1);
            // Add vertices to array
            vertexArr.append(v1);
            vertexArr.append(v4);
            vertexArr.append(v2);
            vertexArr.append(v1);
            vertexArr.append(v3);
            vertexArr.append(v4);
        }
    }
}

/**
 * @brief Cylinder::calcCircleBound Calculates a point at a layer (circle) of the cylinder and its normal
 * @param posit Cylinder position
 * @param r radius of circle at h
 * @param theta rotation angle
 * @param h height
 * @return Vertex of circle at height h and direction of angle
 */
Vertex Cylinder::calcCircleBound(float r1, float theta, float h) {
    // point position
    QVector3D p = QVector3D();
    p.setX(posit.x() + r1*cos(theta));
    p.setY(posit.y() + r1*sin(theta));
    p.setZ(posit.z() + h);

    float a = h + r1*tan(angle); // sphere height
    QVector3D q = posit; // sphere center
    q.setZ(posit.z()+a);
    QVector3D n = (q-p).normalized(); // direction of the normal

    return Vertex(p, n);
}
