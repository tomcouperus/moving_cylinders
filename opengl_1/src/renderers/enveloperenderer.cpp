#include "enveloperenderer.h"

/**
 * @brief EnvelopeRenderer::EnvelopeRenderer Creates a new envelope renderer.
 */
EnvelopeRenderer::EnvelopeRenderer()
{
    envelope = nullptr;
    modelTransform.setToIdentity();
}

/**
 * @brief EnvelopeRenderer::EnvelopeRenderer Creates a new envelope renderer with an envelope.
 * @param env Envelope.
 */
EnvelopeRenderer::EnvelopeRenderer(Envelope *env)
{
    envelope = env;
    modelTransform.setToIdentity();
}

/**
 * @brief EnvelopeRenderer::~EnvelopeRenderer Destroys the envelope renderer.
 */
EnvelopeRenderer::~EnvelopeRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoEnv);
    gl->glDeleteBuffers(1, &vboEnv);
    gl->glDeleteVertexArrays(1, &vaoCenters);
    gl->glDeleteBuffers(1, &vboCenters);
    gl->glDeleteVertexArrays(1, &vaoGrazingCurve);
    gl->glDeleteBuffers(1, &vboGrazingCurve);
    gl->glDeleteVertexArrays(1, &vaoNormals);
    gl->glDeleteBuffers(1, &vboNormals);
}

/**
 * @brief EnvelopeRenderer::initShaders Initialises the shader for the envelope renderer.
 */
void EnvelopeRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();
}

/**
 * @brief EnvelopeRenderer::initBuffers Initialises the buffers for the envelope renderer.
 */
void EnvelopeRenderer::initBuffers()
{
    // Create a vertex array object and a vertex buffer object for the envelope
    gl->glGenVertexArrays(1, &vaoEnv);
    gl->glBindVertexArray(vaoEnv);
    gl->glGenBuffers(1, &vboEnv);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboEnv);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    // Create a vertex array object and a vertex buffer object for the centers
    gl->glGenVertexArrays(1, &vaoCenters);
    gl->glBindVertexArray(vaoCenters);
    gl->glGenBuffers(1, &vboCenters);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboCenters);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    // Create a vertex array object and a vertex buffer object for the grazing curve
    gl->glGenVertexArrays(1, &vaoGrazingCurve);
    gl->glBindVertexArray(vaoGrazingCurve);
    gl->glGenBuffers(1, &vboGrazingCurve);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    // Create a vertex array object and a vertex buffer object for the normals
    gl->glGenVertexArrays(1, &vaoNormals);
    gl->glBindVertexArray(vaoNormals);
    gl->glGenBuffers(1, &vboNormals);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboNormals);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
}

/**
 * @brief EnvelopeRenderer::updateBuffers Updates the buffers with the set envelope.
 */
void EnvelopeRenderer::updateBuffers()
{
    qDebug() << "EnvelopeRenderer::updateBuffers";
    QVector<Vertex>& vertexArrEnv = envelope->getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrEnv.size() * sizeof(Vertex), vertexArrEnv.data(), GL_STATIC_DRAW);

    QVector<Vertex>& vertexArrCenters = envelope->getVertexArrCenters();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrCenters.size() * sizeof(Vertex), vertexArrCenters.data(), GL_STATIC_DRAW);

    QVector<Vertex>& vertexArrGrazingCurve = envelope->getVertexArrGrazingCurve();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrGrazingCurve.size() * sizeof(Vertex), vertexArrGrazingCurve.data(), GL_STATIC_DRAW);

    QVector<Vertex>& vertexArrNormals = envelope->getVertexArrNormalsAt(settings->time);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrNormals.size() * sizeof(Vertex), vertexArrNormals.data(), GL_STATIC_DRAW);
}

/**
 * @brief EnvelopeRenderer::updateUniforms Updates the uniforms for the envelope renderer.
 * @param envelopeTransf Envelope transformation matrix.
 * @param projTransf Projection transformation matrix.
 */
void EnvelopeRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("projTransform", projTransform);
    shader.release();
}

/**
 * @brief EnvelopeRenderer::paintGL Draws the envelope, centers and grazing 
 * curve according to the settings.
 */
void EnvelopeRenderer::paintGL()
{
    shader.bind();

    if(settings->showEnvelope){
        qDebug() << "EnvelopeRenderer::paintGL envelope";
        // Bind envelope buffer
        gl->glBindVertexArray(vaoEnv);
        // Draw envelope
        gl->glDrawArrays(GL_TRIANGLES,0,vertexArrEnv.size());
    }

    if(settings->showToolAxis){
        qDebug() << "EnvelopeRenderer::paintGL axis";
        // Bind centers buffer
        gl->glBindVertexArray(vaoCenters);
        // Draw centers
        gl->glDrawArrays(GL_LINES,0,vertexArrCenters.size());
    }

    if(settings->showGrazingCurve){
        qDebug() << "EnvelopeRenderer::paintGL grazing";
        // Bind grazing curve buffer
        gl->glBindVertexArray(vaoGrazingCurve);
        // Draw grazing curve
        gl->glDrawArrays(GL_LINES,0,vertexArrGrazingCurve.size());
    }

    if(settings->showNormals){
        qDebug() << "EnvelopeRenderer::paintGL normals";
        // Bind normals buffer
        gl->glBindVertexArray(vaoNormals);
        // Draw normals
        gl->glDrawArrays(GL_LINES,0,vertexArrNormals.size());
    }

    shader.release();
}

