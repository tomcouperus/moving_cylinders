
#include "cylinderrenderer.h"
#include <QOpenGLFunctions_3_3_Core>

CylinderRenderer::CylinderRenderer()
{

}

CylinderRenderer::~CylinderRenderer()
{
}


void CylinderRenderer::initCylinder()
{
    // Create a cylinder
    cylinder.initCylinder();
    vertexArrCyl.clear();
    vertexArrCyl = cylinder.getVertices();

}

void CylinderRenderer::initBuffers(){
    // Create a vertex array object
    glGenVertexArrays(1, &vaoCyl);
    glBindVertexArray(vaoCyl);

    // Create a vertex buffer object
    glGenBuffers(1, &vboCyl);
    glBindBuffer(GL_ARRAY_BUFFER, vboCyl);

    // Copy the vertex data to the VBO
    glBufferData(GL_ARRAY_BUFFER, vertexArrCyl.size() * sizeof(Vertex), vertexArrCyl.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}
