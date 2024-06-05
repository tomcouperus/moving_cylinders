#ifndef MOVERENDERER_H
#define MOVERENDERER_H

#include "renderer.h"
#include "../movement/cylindermovement.h"

class PathRenderer  : public Renderer
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
    PathRenderer();
    PathRenderer(CylinderMovement *move);
    ~PathRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers(CylinderMovement *move);
    void updateUniforms(QMatrix4x4 pathTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setMovement(CylinderMovement *move) { this->move = move; }
};

#endif // MOVERENDERER_H
