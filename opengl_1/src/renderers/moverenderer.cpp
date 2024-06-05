#include "moverenderer.h"

MoveRenderer::MoveRenderer()
{
    this->move = nullptr;
    pathTransf.setToIdentity();
}

MoveRenderer::MoveRenderer(CylinderMovement *move)
{
    this->move = move;
    pathTransf.setToIdentity();
}

MoveRenderer::~MoveRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoPath);
    gl->glDeleteBuffers(1, &vboPath);
}

void MoveRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();

    modelLocation = shader.uniformLocation("modelTransform");
    projLocation = shader.uniformLocation("projTransform");
}

void MoveRenderer::initBuffers()
{
    vertexArrPath.clear();
    vertexArrPath = move->getPath().getVertexArr();

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

void MoveRenderer::updateBuffers(CylinderMovement *move)
{
    vertexArrPath.clear();
    vertexArrPath = move->getPath().getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboPath);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrPath.size() * sizeof(Vertex), vertexArrPath.data(), GL_STATIC_DRAW);
}

void MoveRenderer::updateUniforms(QMatrix4x4 pathTransf, QMatrix4x4 projTransf)
{
    shader.bind();
    shader.setUniformValue(modelLocation, pathTransf);
    shader.setUniformValue(projLocation, projTransf);
    shader.release();
}

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
