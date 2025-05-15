#ifndef TOOLRENDERER_H
#define TOOLRENDERER_H

#include "renderer.h"
#include "../tools/tool.h"
#include "../tools/sphere.h"
#include "../tools/cylinder.h"
#include "../tools/drum.h"

/**
 * @brief The ToolRenderer class is a renderer for the tool.
*/
class ToolRenderer : public Renderer
{
    Tool *tool;
    GLuint vaoTool;
    GLuint vboTool;

    Sphere sphere;
    GLuint vaoSph;
    GLuint vboSph;

    QOpenGLShaderProgram shader;

    QMatrix4x4 toolTransform;

public:
    ToolRenderer();
    ToolRenderer(Tool *tool);
    ToolRenderer(Cylinder *cylinder);
    ToolRenderer(Drum *drum);
    ~ToolRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setTool(Tool *tool) { this->tool = tool; }
    inline void setToolTransf(QMatrix4x4 toolTransf) { toolTransform = toolTransf; }


};

#endif // TOOLRENDERER_H
