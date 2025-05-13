#include "drum.h"

/**
 * @brief Drum::Drum Creates a new drum with default values.
 */
Drum::Drum()
    : Tool(ToolType::Tool_Drum), r0(2), rm(0.5)

{
    float phi = asin(height/(2*r0));
    this->a1 = (r0-rm)*tan(phi);
    this->a0 = -a1;
}

/**
 * @brief Drum::Drum Creates a new drum with the given values.
 * @param curveRadius Radius of the drum.
 * @param midRadius Mid radius of the drum.
 * @param height Height of the drum.
 * @param sectors Number of sectors of the drum.
 * @param position Position of the drum.
 */
Drum::Drum(float curveRadius, float midRadius, float height, int sectors, QVector3D position)
    : Tool(ToolType::Tool_Drum), r0(curveRadius), rm(midRadius)
{
    this->sectors = sectors;
    this->posit = position;
    float phi = asin(height/(2*r0));
    this->a1 = (r0-rm)*tan(phi);
    this->a0 = -a1;
}

/**
 * @brief Drum::initDrum Initialises the drum.
*/
void Drum::initDrum(){
    float phi = asin(height/(2*r0));
    computeDrum();
}

void Drum::setSectors(int sectors){
    this->sectors = sectors;
}

void Drum::setRadius(float radius){
    this->r0 = radius;
    float phi = asin(height/(2*r0));
    this->a1 = (r0-rm)*tan(phi);
    this->a0 = -a1;
}

void Drum::setMidRadius(float radius){
    this->rm = radius;
    float phi = asin(height/(2*r0));
    this->a1 = (r0-rm)*tan(phi);
    this->a0 = -a1;
}

void Drum::setHeight(float height){
    this->height = height;
    float phi = asin(height/(2*r0));
    this->a1 = (r0-rm)*tan(phi);
    this->a0 = -a1;
}

void Drum::setPosit(QVector3D position){
    posit = position;
}

void Drum::update() {
    computeDrum();
}

/**
 * @brief Drum::computeDrum computes the vertex array for the cylinder.
 */
void Drum::computeDrum(){
    vertexArr.clear();
    Vertex v1, v2, v3, v4;
    float aDelta = (this->a1 - this->a0)/sectors;
    float a0, a1 = this->a0;
    float theta = 2*PI/sectors;
    for(size_t j=0;j<sectors;j++){
        // transition to next layer
        a0 = a1;
        a1 += aDelta;
        for(size_t i=0;i<sectors;i++){
            // Calculate vertices of a quad of the drum
            v1 = calcCircleBound(theta*i, a0);
            v2 = calcCircleBound(theta*i, a1);
            v3 = calcCircleBound(theta*(i+1), a0);
            v4 = calcCircleBound(theta*(i+1), a1);
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
 * @param theta rotation angle
 * @param a height of sphere
 * @return Vertex of circle and direction of normal
 */
Vertex Drum::calcCircleBound(float theta, float a) {

    float b = r0 - rm;
    float phi = atan(a/b);
    QVector3D direction;
    direction.setX(getRadiusAt(a)*cos(phi)*cos(theta));
    direction.setY(getRadiusAt(a)*cos(phi)*sin(theta));
    direction.setZ(getRadiusAt(a)*sin(phi));
    QVector3D spherePosit = (a)*axisVector;
    QVector3D n = direction.normalized();

    // point position
    QVector3D p = spherePosit + direction;

    return Vertex(p, n);
}
