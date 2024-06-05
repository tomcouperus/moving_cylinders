#include "enveloperenderer.h"

EnvelopeRenderer::EnvelopeRenderer()
{
    envelope = nullptr;
    envelopeTransf.setToIdentity();
}

EnvelopeRenderer::EnvelopeRenderer(Envelope *env)
{
    envelope = env;
    envelopeTransf.setToIdentity();
}

EnvelopeRenderer::~EnvelopeRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoEnv);
    gl->glDeleteBuffers(1, &vboEnv);
    gl->glDeleteVertexArrays(1, &vaoCenters);
    gl->glDeleteBuffers(1, &vboCenters);
    gl->glDeleteVertexArrays(1, &vaoGrazingCurve);
    gl->glDeleteBuffers(1, &vboGrazingCurve);
}

void EnvelopeRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();

    // Get the locations of the model and projection matrices
    modelLocation = shader.uniformLocation("modelTransform");
    projLocation = shader.uniformLocation("projTransform");
}

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
}

void EnvelopeRenderer::updateBuffers(Envelope *env)
{
    envelope = env;
    vertexArrEnv.clear();
    vertexArrEnv = envelope->getVertexArr();
    qDebug() << vertexArrEnv.size();

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
}

void EnvelopeRenderer::updateUniforms(QMatrix4x4 envelopeTransf, QMatrix4x4 projTransf)
{
    shader.bind();
    shader.setUniformValue(modelLocation, envelopeTransf);
    shader.setUniformValue(projLocation, projTransf);
    shader.release();
}

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
    }

    if(settings->showGrazingCurve){
        // Bind grazing curve buffer
        gl->glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
        gl->glUniformMatrix4fv(modelLocation, 1, GL_FALSE, envelopeTransf.data());
        gl->glBindVertexArray(vaoGrazingCurve);
        // Draw grazing curve
        gl->glDrawArrays(GL_LINES,0,vertexArrGrazingCurve.size());
    }

    shader.release();
}

