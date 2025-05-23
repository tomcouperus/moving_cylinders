#include "moverenderer.h"

/**
 * @brief MoveRenderer::MoveRenderer Creates a new move renderer.
 */
MoveRenderer::MoveRenderer() : move(nullptr) {}

/**
 * @brief MoveRenderer::MoveRenderer Creates a new move renderer with a movement.
 * @param move Movement.
 */
MoveRenderer::MoveRenderer(CylinderMovement *move) : move(move) {}

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
}

/**
 * @brief MoveRenderer::initBuffers Initialises the buffers for the move renderer.
 */
void MoveRenderer::initBuffers()
{
    // Create a vertex array object and vertex buffer for the path
    gl->glGenVertexArrays(1, &vaoPath);
    gl->glBindVertexArray(vaoPath);
    gl->glGenBuffers(1, &vboPath);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboPath);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

    gl->glBindVertexArray(0);
}

/**
 * @brief MoveRenderer::updateBuffers Updates the buffers with the set movement.
 */
void MoveRenderer::updateBuffers()
{
    QVector<Vertex>& vertexArrPath = move->getPathVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboPath);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrPath.size() * sizeof(Vertex), vertexArrPath.data(), GL_STATIC_DRAW);
}

/**
 * @brief MoveRenderer::updateUniforms Updates the uniforms of the shader.
 * @param pathTransf Transformation matrix of the movement.
 * @param projTransf Projection matrix.
 */
void MoveRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("projTransform", projTransform);
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
        qDebug() << "MoveRenderer::paintGL";
        gl->glBindVertexArray(vaoPath);
        gl->glDrawArrays(GL_LINE_STRIP, 0, move->getPathVertexArr().size());
    }

    gl->glBindVertexArray(0);

    shader.release();
}
