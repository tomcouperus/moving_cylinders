#include "enveloperenderer.h"

/**
 * @brief EnvelopeRenderer::EnvelopeRenderer Creates a new envelope renderer.
 */
EnvelopeRenderer::EnvelopeRenderer()
{
    envelope = nullptr;
    envelopeTransf.setToIdentity();
}

/**
 * @brief EnvelopeRenderer::EnvelopeRenderer Creates a new envelope renderer with an envelope.
 * @param env Envelope.
 */
EnvelopeRenderer::EnvelopeRenderer(Envelope *env)
{
    envelope = env;
    envelopeTransf.setToIdentity();
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
    gl->glDeleteVertexArrays(1, &vaoAxis);
    gl->glDeleteBuffers(1, &vboAxis);
}

/**
 * @brief EnvelopeRenderer::initShaders Initialises the shader for the envelope renderer.
 */
void EnvelopeRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();

    // Get the locations of the model and projection matrices
    modelLocation = shader.uniformLocation("modelTransform");
    projLocation = shader.uniformLocation("projTransform");
}

/**
 * @brief EnvelopeRenderer::initBuffers Initialises the buffers for the envelope renderer.
 */
void EnvelopeRenderer::initBuffers()
{
    vertexArrEnv.clear();
    vertexArrEnv = envelope->getVertexArr();

    // Create a vertex array object and a vertex buffer object for the envelope
    gl->glGenVertexArrays(1, &vaoEnv);
    gl->glBindVertexArray(vaoEnv);
    gl->glGenBuffers(1, &vboEnv);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrEnv.size() * sizeof(Vertex), vertexArrEnv.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    vertexArrCenters.clear();
    vertexArrCenters = envelope->getVertexArrCenters();

    // Create a vertex array object and a vertex buffer object for the centers
    gl->glGenVertexArrays(1, &vaoCenters);
    gl->glBindVertexArray(vaoCenters);
    gl->glGenBuffers(1, &vboCenters);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrCenters.size() * sizeof(Vertex), vertexArrCenters.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    vertexArrGrazingCurve.clear();
    vertexArrGrazingCurve = envelope->getVertexArrGrazingCurve();

    // Create a vertex array object and a vertex buffer object for the grazing curve
    gl->glGenVertexArrays(1, &vaoGrazingCurve);
    gl->glBindVertexArray(vaoGrazingCurve);
    gl->glGenBuffers(1, &vboGrazingCurve);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrGrazingCurve.size() * sizeof(Vertex), vertexArrGrazingCurve.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    vertexArrNormals.clear();
    vertexArrNormals = envelope->getVertexArrNormalsAt(settings->time);

    // Create a vertex array object and a vertex buffer object for the normals
    gl->glGenVertexArrays(1, &vaoNormals);
    gl->glBindVertexArray(vaoNormals);
    gl->glGenBuffers(1, &vboNormals);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrNormals.size() * sizeof(Vertex), vertexArrNormals.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    vertexArrAxis.clear();
    vertexArrAxis = envelope->getVertexArrToolAxis();

    // Create a vertex array object and a vertex buffer object for the axis
    gl->glGenVertexArrays(1, &vaoAxis);
    gl->glBindVertexArray(vaoAxis);
    gl->glGenBuffers(1, &vboAxis);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboAxis);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrAxis.size() * sizeof(Vertex), vertexArrAxis.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
}

/**
 * @brief EnvelopeRenderer::updateBuffers Updates the buffers with the new envelope.
 * @param env Envelope.
 */
void EnvelopeRenderer::updateBuffers(Envelope *env)
{
    envelope = env;
    vertexArrEnv.clear();
    vertexArrEnv = envelope->getVertexArr();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrEnv.size() * sizeof(Vertex), vertexArrEnv.data(), GL_STATIC_DRAW);

    vertexArrCenters.clear();
    vertexArrCenters = envelope->getVertexArrCenters();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrCenters.size() * sizeof(Vertex), vertexArrCenters.data(), GL_STATIC_DRAW);

    vertexArrGrazingCurve.clear();
    vertexArrGrazingCurve = envelope->getVertexArrGrazingCurve();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrGrazingCurve.size() * sizeof(Vertex), vertexArrGrazingCurve.data(), GL_STATIC_DRAW);

    vertexArrNormals.clear();
    vertexArrNormals = envelope->getVertexArrNormalsAt(settings->time);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrNormals.size() * sizeof(Vertex), vertexArrNormals.data(), GL_STATIC_DRAW);

    vertexArrAxis.clear();
    vertexArrAxis = envelope->getVertexArrToolAxis();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboAxis);
    gl->glBufferData(GL_ARRAY_BUFFER, vertexArrAxis.size() * sizeof(Vertex), vertexArrAxis.data(), GL_STATIC_DRAW);
}

/**
 * @brief EnvelopeRenderer::updateUniforms Updates the uniforms for the envelope renderer.
 * @param envelopeTransf Envelope transformation matrix.
 * @param projTransf Projection transformation matrix.
 */
void EnvelopeRenderer::updateUniforms(QMatrix4x4 envelopeTransf, QMatrix4x4 projTransf)
{
    shader.bind();
    shader.setUniformValue(modelLocation, envelopeTransf);
    shader.setUniformValue(projLocation, projTransf);
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
        // Bind envelope buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoEnv);
        // Draw envelope
        gl->glDrawArrays(GL_TRIANGLES,0,vertexArrEnv.size());
    }

    if(settings->showToolAxis){
        // Bind centers buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoCenters);
        // Draw centers
        gl->glDrawArrays(GL_LINES,0,vertexArrCenters.size());

        // Bind axis buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboAxis);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoAxis);
        // Draw axis
        gl->glDrawArrays(GL_LINES,0,vertexArrAxis.size());
    }

    if(settings->showGrazingCurve){
        // Bind grazing curve buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoGrazingCurve);
        // Draw grazing curve
        gl->glDrawArrays(GL_LINES,0,vertexArrGrazingCurve.size());
    }

    if(settings->showNormals){
        // Bind normals buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoNormals);
        // Draw normals
        gl->glDrawArrays(GL_LINES,0,vertexArrNormals.size());
    }

    shader.release();
}

