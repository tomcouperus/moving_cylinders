#ifndef ENVELOPERENDERER_H
#define ENVELOPERENDERER_H

#include "../envelope.h"
#include "renderer.h"

class EnvelopeRenderer : public Renderer
{
    Envelope *envelope;
    QOpenGLShaderProgram shader;

    GLuint vaoEnv;
    GLuint vboEnv;
    QVector<Vertex> vertexArrEnv;

    GLuint vboCenters;
    GLuint vaoCenters;
    QVector<Vertex> vertexArrCenters;

    GLuint vboGrazingCurve;
    GLuint vaoGrazingCurve;
    QVector<Vertex> vertexArrGrazingCurve;

    QMatrix4x4 envelopeTransf;
    QMatrix4x4 projTransf;

    GLint modelLocation;
    GLint projLocation;
public:
    EnvelopeRenderer();
    EnvelopeRenderer(Envelope *env);
    ~EnvelopeRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers(Envelope *env);
    void updateUniforms(QMatrix4x4 envelopeTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setEnvelope(Envelope *env) { this->envelope = env; }
    inline void setTransf(QMatrix4x4 envelopeTransf) { this->envelopeTransf = envelopeTransf; }
};

#endif // ENVELOPERENDERER_H
