#include "toolrenderer.h"

/**
 * @brief ToolRenderer::ToolRenderer Creates a new tool renderer.
 */
ToolRenderer::ToolRenderer() : tool(nullptr)
{
    modelTransform.setToIdentity();
}

/**
 * @brief ToolRenderer::ToolRenderer Creates a new tool renderer with a cylinder.
 * @param tool Tool.
 */
ToolRenderer::ToolRenderer(Tool *tool) : tool(tool) {}

/**
 * @brief ToolRenderer::~ToolRenderer Deconstructs the tool renderer.
 */
ToolRenderer::~ToolRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoTool);
    gl->glDeleteBuffers(1, &vboTool);
    gl->glDeleteVertexArrays(1, &vaoSph);
    gl->glDeleteBuffers(1, &vboSph);
}

/**
 * @brief ToolRenderer::initShaders Initialises the shaders for the tool renderer.
 */
void ToolRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    qDebug() << "shader link";
    shader.link();
    qDebug() << "shader linked";
}

/**
 * @brief ToolRenderer::initBuffers Initialises the buffers for the tool renderer.
 */
void ToolRenderer::initBuffers()
{
    // Create a vertex array object and vertex buffer for the tool
    gl->glGenVertexArrays(1, &vaoTool);
    gl->glBindVertexArray(vaoTool);
    gl->glGenBuffers(1, &vboTool);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);

    // Set the vertex attribute pointers
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

    // Create a vertex array object and vertex buffer for the sphere
    gl->glGenVertexArrays(1, &vaoSph);
    gl->glBindVertexArray(vaoSph);
    gl->glGenBuffers(1, &vboSph);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboSph);

    // Set the vertex attribute pointers
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

    // Unbind vertex array
    gl->glBindVertexArray(0);
}

/**
 * @brief ToolRenderer::updateBuffers Updates the buffers for the tool renderer with the set tool
 */
void ToolRenderer::updateBuffers()
{
    qDebug()<< "ToolRenderer::updateBuffers";
    QVector<Vertex>& vertexArrTool = tool->getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);

    QVector3D sphPosit = tool->getPosition()+ tool->getSphereCenterHeightAt(settings->a())*tool->getAxisVector();
    sphere.setPosition(sphPosit);
    sphere.setRadius(tool->getSphereRadiusAt(settings->a()));
    sphere.generateVertexArr(20, 20);
    QVector<Vertex>& vertexArrSph = sphere.getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboSph);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrSph.size() * sizeof(Vertex), vertexArrSph.data(), GL_STATIC_DRAW);
}

/**
 * @brief ToolRenderer::updateUniforms Updates the uniforms for the tool renderer.
 * @param toolTransf Tool transformation matrix.
 * @param projTransf Projection transformation matrix.
 */
void ToolRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform * toolTransform);
    shader.setUniformValue("projTransform", projTransform);
    shader.release();
}

/**
 * @brief ToolRenderer::paintGL Draws the tool.
 */
void ToolRenderer::paintGL()
{
    shader.bind();

    if(settings->showTool){
        qDebug() << "ToolRenderer::paintGL tool";
        // Bind cylinder buffer
        gl->glBindVertexArray(vaoTool);
        // Draw cylinder
        gl->glDrawArrays(GL_TRIANGLES,0,tool->getVertexArr().size());
    }

    if (settings->showSpheres)
    {
        qDebug() << "ToolRenderer::paintGL sphere";
        // Bind sphere buffer
        gl->glBindVertexArray(vaoSph);
        // Draw sphere
        gl->glDrawArrays(GL_LINES,0,sphere.getVertexArr().size());
    }

    gl->glBindVertexArray(0);
    
    shader.release();
}

