#ifndef MOVERENDERER_H
#define MOVERENDERER_H

#include "renderer.h"
#include "../movement/cylindermovement.h"

/**
 * @brief The MoveRenderer class is a renderer for the movement of the tool.
 */
class MoveRenderer  : public Renderer
{
    CylinderMovement *move;

    GLuint vaoPath;
    GLuint vboPath;

    QOpenGLShaderProgram shader;

public:
    MoveRenderer();
    MoveRenderer(CylinderMovement *move);
    ~MoveRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setMovement(CylinderMovement *move) { this->move = move; }
};

#endif // MOVERENDERER_H
