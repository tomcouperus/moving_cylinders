#include "cylinder.h"

Cylinder::Cylinder()
{
    r = 0.5;
    angle = 0.5;
    h = 2;
    sectors = 20;
    posit = QVector3D(0.0f,0.0f,0.0f);
    theta = 2*PI/sectors;
}

Cylinder::Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position)
    : r(baseRadius), angle(angle), h(height), sectors(sectors),posit(position)
{
    theta = 2*PI/sectors;
}

void Cylinder::initCylinder(){
    Vertex v1, v2, v3, v4;
    float r0 = r;
    float r1 = r + h*tan(angle);
    QVector3D color = QVector3D(0.0f,0.0f,1.0f);
    for(size_t i=0;i<sectors;i++){
        v1 = Vertex(posit, color);
        v1.xCoord += r0*cos(theta*i);
        v1.yCoord += r0*sin(theta*i);

        v2 = Vertex(posit, color);
        v2.xCoord += r1*cos(theta*i);
        v2.yCoord += r1*sin(theta*i);
        v2.zCoord += h;

        v3 = Vertex(posit, color);
        v3.xCoord += r0*cos(theta*(i+1));
        v3.yCoord += r0*sin(theta*(i+1));

        v4 = Vertex(posit, color);
        v4.xCoord += r1*cos(theta*(i+1));
        v4.yCoord += r1*sin(theta*(i+1));
        v4.zCoord += h;

        vertexArr.append(v1);
        vertexArr.append(v4);
        vertexArr.append(v2);
        vertexArr.append(v1);
        vertexArr.append(v3);
        vertexArr.append(v4);
    }

}
