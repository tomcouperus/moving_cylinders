#ifndef MOVERENDERER_H
#define MOVERENDERER_H

#include "renderer.h"
#include "../movement/cylindermovement.h"

class MoveRenderer  : public Renderer
{
    CylinderMovement *move;

    GLuint vaoPath;
    GLuint vboPath;
    QVector<Vertex> vertexArrPath;

    QOpenGLShaderProgram shader;

    GLint modelLocation;
    GLint projLocation;

    QMatrix4x4 pathTransf;
    QMatrix4x4 projTransf;

public:
    MoveRenderer();
    MoveRenderer(CylinderMovement *move);
    ~MoveRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers(CylinderMovement *move);
    void updateUniforms(QMatrix4x4 pathTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setMovement(CylinderMovement *move) { this->move = move; }
    inline void setTransf(QMatrix4x4 moveTransf) { this->pathTransf = moveTransf; }
};

#endif // MOVERENDERER_H
