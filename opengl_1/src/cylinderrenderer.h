#ifndef CYLINDERRENDERER_H
#define CYLINDERRENDERER_H


#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

#include "model.h"
#include "cylinder.h"
#include "vertex.h"

class CylinderRenderer
{
    Cylinder cylinder;
    GLuint vaoCyl;
    GLuint vboCyl;
    QVector<Vertex> vertexArrCyl;
    QOpenGLShaderProgram shader;
    GLint modelLocation;
    GLint projLocation;


    // Transformation matrices for the model
    QMatrix4x4 modelScaling;
    QMatrix4x4 modelRotation;
    QMatrix4x4 cylinderRotation;
    QMatrix4x4 cylinderTranslation;
    QMatrix4x4 modelTranslation;
    QMatrix4x4 cylinderTransf;
public:
    CylinderRenderer();
    ~CylinderRenderer();

    void initCylinder();
    void initBuffers();
    void updateBuffers();
    void updateUniforms();

    void createShaderProgram();
    void paintGL();
};

#endif // CYLINDERRENDERER_H
