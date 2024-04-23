#include "pyramid.h"

/**
 * @brief Pyramid::Pyramid Creates a new pyramid.
 */
Pyramid::Pyramid(){}

/**
 * @brief Pyramid::initPyramid Generates the pyramid coordinates
 */
void Pyramid::initPyramid()
{
    Vertex v1 = Vertex(0.0f,0.0f,-1.0f,1.0f,0.0f,1.0f); //pink
    Vertex v2 = Vertex(-1.0f,1.0f,1.0f,1.0f,0.0f,0.0f); //red
    Vertex v3 = Vertex(1.0f,1.0f,1.0f,0.0f,1.0f,0.0f); //green
    Vertex v4 = Vertex(-1.0f,-1.0f,1.0f,0.0f,0.0f,1.0f); //blue
    Vertex v5 = Vertex(1.0f,-1.0f,1.0f,1.0f,1.0f,0.0f); //yellow

    vertexArr = QVector({ v1, v2, v3,
                         v1, v3, v5,
                         v1, v5, v4,
                         v1, v4, v2,
                         v2, v4, v5,
                         v5, v3, v2  });
}
