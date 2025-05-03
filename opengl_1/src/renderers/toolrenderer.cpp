#include "toolrenderer.h"

/**
 * @brief ToolRenderer::ToolRenderer Creates a new tool renderer.
 */
ToolRenderer::ToolRenderer()
{
    modelTransform.setToIdentity();
}

/**
 * @brief ToolRenderer::ToolRenderer Creates a new tool renderer with a cylinder.
 * @param cyl Cylinder.
 */
ToolRenderer::ToolRenderer(Cylinder *cyl)
{
    tool = cyl;
    QVector3D sphPosit = tool->getPosition()+(tool->getA0()+settings->a)*tool->getAxisVector();
    sphere = Sphere(sphPosit, tool->getRadiusAt(tool->getA0()+settings->a));
    vertexArrSph.clear();
    sphere.generateVertexArr(20, 20);
    vertexArrSph = sphere.getVertexArr();
    modelTransform.setToIdentity();
}

/**
 * @brief ToolRenderer::ToolRenderer Creates a new tool renderer with a drum.
 * @param drum Drum.
 */
ToolRenderer::ToolRenderer(Drum *drum)
{
    tool = drum;
    QVector3D sphPosit = tool->getPosition()+(tool->getA0()+settings->a)*tool->getAxisVector();
    sphere = Sphere(sphPosit, tool->getRadiusAt(tool->getA0()+settings->a));
    vertexArrSph.clear();
    sphere.generateVertexArr(20, 20);
    vertexArrSph = sphere.getVertexArr();
    modelTransform.setToIdentity();
}

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

    QVector3D sphPosit = tool->getPosition()+(tool->getA0()+settings->a)*tool->getAxisVector();
    sphere.setPosition(sphPosit);
    sphere.setRadius(tool->getRadiusAt(tool->getA0()+settings->a));
    vertexArrSph.clear();
    sphere.generateVertexArr(20, 20);
    vertexArrSph = sphere.getVertexArr();

    gl->glGenVertexArrays(1, &vaoSph);
    gl->glBindVertexArray(vaoSph);

    gl->glGenBuffers(1, &vboSph);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboSph);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrSph.size() * sizeof(Vertex), vertexArrSph.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    gl->glEnableVertexAttribArray(1);
}

/**
 * @brief ToolRenderer::updateBuffers Updates the buffers for the tool renderer with the set tool
 */
void ToolRenderer::updateBuffers()
{
    qDebug()<< "ToolRenderer::updateBuffers";
    vertexArrTool.clear();
    vertexArrTool = tool->getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);

    QVector3D sphPosit = tool->getPosition()+(tool->getA0()+settings->a)*tool->getAxisVector();
    sphere.setPosition(sphPosit);
    sphere.setRadius(tool->getRadiusAt(tool->getA0()+settings->a));
    vertexArrSph.clear();
    sphere.generateVertexArr(20, 20);
    vertexArrSph = sphere.getVertexArr();

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
        gl->glDrawArrays(GL_TRIANGLES,0,vertexArrTool.size());
    }

    if (settings->showSpheres)
    {
        qDebug() << "ToolRenderer::paintGL sphere";
        // Bind sphere buffer
        gl->glBindVertexArray(vaoSph);
        // Draw sphere
        gl->glDrawArrays(GL_LINES,0,vertexArrSph.size());
    }
    
    shader.release();
}

