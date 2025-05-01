#ifndef ENVELOPERENDERER_H
#define ENVELOPERENDERER_H

#include "../envelope.h"
#include "renderer.h"

/**
 * @brief The EnvelopeRenderer class is a renderer for the envelope of the tool.
 */
class EnvelopeRenderer : public Renderer
{
    Envelope *envelope;
    QOpenGLShaderProgram shader;

    GLuint vaoEnv;
    GLuint vboEnv;
    QVector<Vertex> vertexArrEnv;

    // Centers of the 2-param family od spheres that describe the envelope
    GLuint vboCenters;
    GLuint vaoCenters;
    QVector<Vertex> vertexArrCenters;

    // Grazing curve of the tool that describes the envelope
    GLuint vboGrazingCurve;
    GLuint vaoGrazingCurve;
    QVector<Vertex> vertexArrGrazingCurve;

    // Normals for debugging
    GLuint vboNormals;
    GLuint vaoNormals;
    QVector<Vertex> vertexArrNormals;


    // Transformation matrices for the envelope
    QMatrix4x4 envelopeTransf;

    GLint modelLocation;
    GLint projLocation;
public:
    EnvelopeRenderer();
    EnvelopeRenderer(Envelope *env);
    ~EnvelopeRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers();
    void updateUniforms(QMatrix4x4 envelopeTransf, QMatrix4x4 projTransf);
    void paintGL();

    inline void setEnvelope(Envelope *env) { this->envelope = env; }
    inline void setTransf(QMatrix4x4 envelopeTransf) { this->envelopeTransf = envelopeTransf; }
};

#endif // ENVELOPERENDERER_H
