#include "drum.h"

Drum::Drum()
    : r0(1.5), rm(0.5)

{
    qDebug()<< "Drum()";
    float phi = asin(height/(2*r0));
    this->a1 = rm*tan(phi);
    this->a0 = -a1;
}

Drum::Drum(float curveRadius, float midRadius, float height, int sectors, QVector3D position)
    : r0(curveRadius), rm(midRadius)
{
    this->sectors = sectors;
    this->posit = position;
    float phi = asin(height/(2*r0));
    this->a1 = rm*tan(phi);
    this->a0 = -a1;
}

void Drum::initDrum(){
    float phi = asin(height/(2*r0));
    qDebug()<< height/(2*r0);
    computeDrum();
}

void Drum::setSectors(int sectors){
    this->sectors = sectors;
    computeDrum();
}
void Drum::setRadius(float radius){
    r0 = radius;
    float phi = asin(height/(2*r0));
    this->a1 = rm*tan(phi);
    this->a0 = -a1;
    computeDrum();
}
void Drum::setMidRadius(float radius){
    this->rm = radius;
    float phi = asin(height/(2*r0));
    this->a1 = rm*tan(phi);
    this->a0 = -a1;
    computeDrum();
}
void Drum::setHeight(float height){
    this->height = height;
    float phi = asin(height/(2*r0));
    this->a1 = rm*tan(phi);
    this->a0 = -a1;
    computeDrum();
}
void Drum::setPosit(QVector3D position){
    posit = position;
    computeDrum();
}

/**
 * @brief Drum::computeDrum computes the vertex array for the cylinder.
 */
void Drum::computeDrum(){
    vertexArr.clear();
    Vertex v1, v2, v3, v4;
    float aDelta = (this->a1 - this->a0)/sectors;
    float r0, r1 = this->r0;
    float a0, a1 = this->a0;
    float theta = 2*PI/sectors;
    for(size_t j=0;j<sectors;j++){
        // transition to next layer
        r0 = r1;
        a0 = a1;
        a1 += aDelta;
        r1 = getRadiusAt(a1);
        for(size_t i=0;i<sectors;i++){
            // Calculate vertices of a quad of the cylinder
            v1 = calcCircleBound(r0, theta*i, a0);
            v2 = calcCircleBound(r1, theta*i, a1);
            v3 = calcCircleBound(r0, theta*(i+1), a0);
            v4 = calcCircleBound(r1, theta*(i+1), a1);
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
 * @brief Drum::calcCircleBound Calculates a point at a layer (circle) of the drum and its normal
 * @param r radius of sphere at a
 * @param theta rotation angle
 * @param a height
 * @return Vertex of circle at height h and direction of angle
 */
Vertex Drum::calcCircleBound(float r1, float theta, float a) {

    float b = r0 - rm;
    QVector3D direction = perpVector;
    direction.setX(direction.x() + r1*cos(theta));
    direction.setY(direction.y() + r1*sin(theta));
    QVector3D curvatureCenter = posit + b*direction;
    QVector3D spherePosit = a*axisVector;
    QVector3D n = (spherePosit - curvatureCenter).normalized();

    // point position
    QVector3D p = spherePosit + r1*n;

    return Vertex(p, n);
}
