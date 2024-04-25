#include "cylinder.h"

/**
 * @brief Cylinder::Cylinder Default constructor builds cylinder to preset size
 */
Cylinder::Cylinder()
{
    r = 0.5;
    angle = 0.5;
    height = 2;
    sectors = 25;
    posit = QVector3D(0.0f,0.0f,0.0f);
    theta = 2*PI/sectors;
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
    : r(baseRadius), angle(angle), height(height), sectors(sectors),posit(position)
{
    theta = 2*PI/sectors;
}

/**
 * @brief Cylinder::initCylinder Initializes the vertex array for the cylinder.
 */
void Cylinder::initCylinder(){
    Vertex v1, v2, v3, v4;
    float hDelta = height/sectors;
    float r0, r1 = r;
    float h0, h1 = 0;
    QVector3D color = QVector3D(0.0f,0.0f,1.0f);
    float c = height/(height*sectors);
    for(size_t j=0;j<sectors;j++){
        // transition to next layer
        h0 = h1;
        h1 += hDelta;
        r0 = r1;
        r1 = r + h1*tan(angle);
        // color for testing
        color += QVector3D(c,0.0,0.0);
        for(size_t i=0;i<sectors;i++){
            // Calculate vertices of a quad of a cylinder
            v1 = Vertex(calcCircleBound(posit,r0,theta*i,h0), color);
            v2 = Vertex(calcCircleBound(posit,r1,theta*i,h1), color);
            v3 = Vertex(calcCircleBound(posit,r0,theta*(i+1),h0), color);
            v4 = Vertex(calcCircleBound(posit,r1,theta*(i+1),h1), color);
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
 * @brief Cylinder::calcCircleBound Calculates a vertex at a layer (circle) of the cylinder
 * @param posit Cylinder position
 * @param r radius of circle at h
 * @param angle rotation angle
 * @param h height
 * @return vertex of circle at height h and direction of angle
 */
QVector3D Cylinder::calcCircleBound(QVector3D posit, float r, float angle, float h) {
    QVector3D v = QVector3D();
    v.setX(posit.x() + r*cos(angle));
    v.setY(posit.y() + r*sin(angle));
    v.setZ(posit.z() + h);
    return v;
}
