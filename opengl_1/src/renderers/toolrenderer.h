#ifndef TOOLRENDERER_H
#define TOOLRENDERER_H

#include "renderer.h"
#include "../tools/tool.h"
#include "../tools/cylinder.h"
#include "../tools/drum.h"
#include "envelope.h"

class ToolRenderer : public Renderer
{
    Tool *tool;
    GLuint vaoTool;
    GLuint vboTool;
    QVector<Vertex> vertexArrTool;
    QOpenGLShaderProgram shader;

    GLint modelLocation;
    GLint projLocation;

    // Transformation matrices for the tool
    QMatrix4x4 toolScaling;
    QMatrix4x4 toolRotation;
    QMatrix4x4 toolTranslation;
    QMatrix4x4 toolTransf;

public:
    ToolRenderer();
    ToolRenderer(Cylinder *cylinder);
    ToolRenderer(Drum *drum);
    ~ToolRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers(Tool *tool);
    void updateUniforms(QMatrix4x4 toolTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setTool(Tool *tool) { this->tool = tool; }
    inline void setScaling(QMatrix4x4 toolScaling) { this->toolScaling = toolScaling; }
    inline void setRotation(QMatrix4x4 toolRotation) { this->toolRotation = toolRotation; }
    inline void setTranslation(QMatrix4x4 toolTranslation) { this->toolTranslation = toolTranslation; }
    inline void setTransf(QMatrix4x4 toolTransf) { this->toolTransf = toolTransf; }


};

#endif // TOOLRENDERER_H
