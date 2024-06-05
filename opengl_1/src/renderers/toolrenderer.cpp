#include "toolrenderer.h"

ToolRenderer::ToolRenderer()
{
    toolTransf.setToIdentity();
}

ToolRenderer::ToolRenderer(Cylinder *cyl)
{
    tool = cyl;
    toolTransf.setToIdentity();
}

ToolRenderer::ToolRenderer(Drum *drum)
{
    tool = drum;
    toolTransf.setToIdentity();
}

ToolRenderer::~ToolRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoTool);
    gl->glDeleteBuffers(1, &vboTool);
}

void ToolRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    qDebug() << "shader link";
    shader.link();
    qDebug() << "shader linked";

    // Get the locations of the model and projection matrices
    modelLocation = shader.uniformLocation("modelTransform");
    projLocation = shader.uniformLocation("projTransform");
}

void ToolRenderer::initBuffers()
{
    vertexArrTool.clear();
    vertexArrTool = tool->getVertexArr();

    // Create a vertex array object
    gl->glGenVertexArrays(1, &vaoTool);
    gl->glBindVertexArray(vaoTool);

    // Create a vertex buffer object
    gl->glGenBuffers(1, &vboTool);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);

    // Copy the vertex data to the VBO
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    gl->glEnableVertexAttribArray(1);
}

void ToolRenderer::updateBuffers(Tool *tool)
{
    qDebug()<< "update buffers";
    vertexArrTool.clear();
    vertexArrTool = tool->getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);
}

void ToolRenderer::updateUniforms(QMatrix4x4 toolTransf, QMatrix4x4 projTransf)
{
    shader.bind();
    shader.setUniformValue(modelLocation, toolTransf);
    shader.setUniformValue(projLocation, projTransf);
    shader.release();
}

void ToolRenderer::paintGL()
{
    qDebug() << "Drawing tool";
    shader.bind();
    // Bind cylinder buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, toolTransf.data());
    gl->glBindVertexArray(vaoTool);
    // Draw cylinder
    gl->glDrawArrays(GL_TRIANGLES,0,vertexArrTool.size());
    shader.release();
}

