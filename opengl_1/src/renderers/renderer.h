#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>

#include "../settings.h"

/**
 * @brief The Renderer class represents a generic renderer class. The class is
 * abstract and should only contain functionality that is applicable
 * to every renderer.
 */
class Renderer {
public:
    Renderer();
    Renderer(QOpenGLFunctions_4_1_Core *functions, Settings *settings);
    ~Renderer();
    void init(QOpenGLFunctions_4_1_Core *f, Settings *s);

    inline void setModelTransf(QMatrix4x4 modelTransf) { this->modelTransform = modelTransf; }
    inline void setProjTransf(QMatrix4x4 projTransf) { this->projTransform = projTransf; }

protected:
    QMatrix4x4 modelTransform, projTransform;

    virtual void initShaders() = 0;
    virtual void initBuffers() = 0;
    virtual void paintGL() = 0;
    virtual void updateBuffers() = 0;
    virtual void updateUniforms() = 0;

    QOpenGLFunctions_4_1_Core *gl;
    Settings *settings;
};
#endif // RENDERER_H
