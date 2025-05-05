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

    // Centers of the 2-param family od spheres that describe the envelope
    GLuint vboCenters;
    GLuint vaoCenters;

    // Grazing curve of the tool that describes the envelope
    GLuint vboGrazingCurve;
    GLuint vaoGrazingCurve;

    // Normals for debugging
    GLuint vboNormals;
    GLuint vaoNormals;

public:
    EnvelopeRenderer();
    EnvelopeRenderer(Envelope *env);
    ~EnvelopeRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setEnvelope(Envelope *env) { this->envelope = env; }
};

#endif // ENVELOPERENDERER_H
