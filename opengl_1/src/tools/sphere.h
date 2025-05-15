#ifndef SPHERE_H
#define SPHERE_H

#include <QVector3D>
#include <QList>
#include "vertex.h"

class Sphere
{
    QVector3D position;
    float radius;

    constexpr static float PI = 3.14159265;

    QVector<Vertex> vertexArr;
public:
    Sphere() : position(QVector3D(0, 0, 0)), radius(1) {}
    Sphere(QVector3D position, float radius) : position(position), radius(radius) {}

    inline QVector<Vertex>& getVertexArr() { return vertexArr; }
    inline QVector3D getPosition() { return position; }
    inline float getRadius() { return radius; }
    inline void setPosition(QVector3D position) { this->position = position; }
    inline void setRadius(float radius) { this->radius = radius; }

    inline void generateVertexArr(int numSlices, int numStacks) {
        vertexArr.clear();
        QVector3D v1, v2, v3, v4;
        for (int i = 0; i < numSlices; i++)
        {
            for (int j = 0; j < numStacks; j++)
            {
                v1 = QVector3D(radius * cos(2 * PI * i / numSlices) * sin(PI * j / numStacks),
                                radius * sin(2 * PI * i / numSlices) * sin(PI * j / numStacks),
                                radius * cos(PI * j / numStacks));
                v2 = QVector3D(radius * cos(2 * PI * (i + 1) / numSlices) * sin(PI * j / numStacks),
                                radius * sin(2 * PI * (i + 1) / numSlices) * sin(PI * j / numStacks),
                                radius * cos(PI * j / numStacks));
                v3 = QVector3D(radius * cos(2 * PI * (i + 1) / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * sin(2 * PI * (i + 1) / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * cos(PI * (j + 1) / numStacks));
                v4 = QVector3D(radius * cos(2 * PI * i / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * sin(2 * PI * i / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * cos(PI * (j + 1) / numStacks));
                
                v1 += position;
                v2 += position;
                v3 += position;
                v4 += position;
                
                vertexArr.push_back(Vertex(v1, QVector3D(0, 1, 0)));
                vertexArr.push_back(Vertex(v2, QVector3D(0, 1, 0)));
                vertexArr.push_back(Vertex(v3, QVector3D(0, 1, 0)));
                vertexArr.push_back(Vertex(v3, QVector3D(0, 1, 0)));
                vertexArr.push_back(Vertex(v4, QVector3D(0, 1, 0)));
                vertexArr.push_back(Vertex(v1, QVector3D(0, 1, 0)));
            }
        }
        
    }
};

#endif // SPHERE_H
