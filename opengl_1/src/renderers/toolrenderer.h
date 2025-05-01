#ifndef TOOLRENDERER_H
#define TOOLRENDERER_H

#include "renderer.h"
#include "../tools/tool.h"
#include "../tools/sphere.h"
#include "../tools/cylinder.h"
#include "../tools/drum.h"
#include "envelope.h"

/**
 * @brief The ToolRenderer class is a renderer for the tool.
*/
class ToolRenderer : public Renderer
{
    Tool *tool;
    GLuint vaoTool;
    GLuint vboTool;
    QVector<Vertex> vertexArrTool;

    Sphere sphere;
    GLuint vaoSph;
    GLuint vboSph;
    QVector<Vertex> vertexArrSph;

    QOpenGLShaderProgram shader;

    GLint modelLocation;
    GLint projLocation;

    // Transformation matrices for the tool
    QMatrix4x4 toolTransf;

public:
    ToolRenderer();
    ToolRenderer(Cylinder *cylinder);
    ToolRenderer(Drum *drum);
    ~ToolRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers();
    void updateUniforms(QMatrix4x4 toolTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setTool(Tool *tool) { this->tool = tool; }
    inline void setTransf(QMatrix4x4 toolTransf) { this->toolTransf = toolTransf; }


};

#endif // TOOLRENDERER_H
