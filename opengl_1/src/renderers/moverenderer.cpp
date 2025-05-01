#include "moverenderer.h"

/**
 * @brief MoveRenderer::MoveRenderer Creates a new move renderer.
 */
MoveRenderer::MoveRenderer()
{
    this->move = nullptr;
    pathTransf.setToIdentity();
}

/**
 * @brief MoveRenderer::MoveRenderer Creates a new move renderer with a movement.
 * @param move Movement.
 */
MoveRenderer::MoveRenderer(CylinderMovement *move)
{
    this->move = move;
    pathTransf.setToIdentity();
}

/**
 * @brief MoveRenderer::~MoveRenderer Destructor of the move renderer.
 */
MoveRenderer::~MoveRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoPath);
    gl->glDeleteBuffers(1, &vboPath);
}

/**
 * @brief MoveRenderer::initShaders Initialises the shaders for the move renderer.
 */
void MoveRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();

    modelLocation = shader.uniformLocation("modelTransform");
    projLocation = shader.uniformLocation("projTransform");
}

/**
 * @brief MoveRenderer::initBuffers Initialises the buffers for the move renderer.
 */
void MoveRenderer::initBuffers()
{
    vertexArrPath.clear();
    vertexArrPath = move->getPathVertexArr();

    gl->glGenVertexArrays(1, &vaoPath);
    gl->glBindVertexArray(vaoPath);

    gl->glGenBuffers(1, &vboPath);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboPath);

    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrPath.size() * sizeof(Vertex), vertexArrPath.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    gl->glEnableVertexAttribArray(1);
}

/**
 * @brief MoveRenderer::updateBuffers Updates the buffers with the set movement.
 */
void MoveRenderer::updateBuffers()
{
    vertexArrPath.clear();
    vertexArrPath = move->getPathVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboPath);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrPath.size() * sizeof(Vertex), vertexArrPath.data(), GL_STATIC_DRAW);
}

/**
 * @brief MoveRenderer::updateUniforms Updates the uniforms of the shader.
 * @param pathTransf Transformation matrix of the movement.
 * @param projTransf Projection matrix.
 */
void MoveRenderer::updateUniforms(QMatrix4x4 pathTransf, QMatrix4x4 projTransf)
{
    shader.bind();
    shader.setUniformValue(modelLocation, pathTransf);
    shader.setUniformValue(projLocation, projTransf);
    shader.release();
}

/**
 * @brief MoveRenderer::paintGL Draws the path that desctibes movement.
 */
void MoveRenderer::paintGL()
{
    shader.bind();
    if(settings->showPath)
    {
        gl->glBindVertexArray(vaoPath);
        gl->glDrawArrays(GL_LINE_STRIP, 0, vertexArrPath.size());
        gl->glBindVertexArray(0);
    }
    shader.release();
}
