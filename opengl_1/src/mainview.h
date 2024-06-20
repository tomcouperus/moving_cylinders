#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

#include "vertex.h"
#include "tools/cylinder.h"
#include "movement/cylindermovement.h"
#include "movement/simplepath.h"
#include "envelope.h"
#include "settings.h"
#include "renderers/toolrenderer.h"
#include "renderers/enveloperenderer.h"
#include "renderers/moverenderer.h"


/**
 * @brief The MainView class is resonsible for the actual content of the main
 * window.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
    friend class MainWindow;

    // Tool rendering
    ToolRenderer toolRend;
    ToolRenderer toolRend2;
    Drum drum;
    Cylinder cylinder;
    Drum drum2;
    Cylinder cylinder2;

    // Path rendering
    CylinderMovement move;
    MoveRenderer movRend;
    CylinderMovement move2;
    MoveRenderer movRend2;

    // Envelope rendering
    EnvelopeRenderer envRend;
    Envelope envelope;
    EnvelopeRenderer envRend2;
    Envelope envelope2;

    // Transformation matrices for the model
    QMatrix4x4 modelScaling;
    QMatrix4x4 modelRotation;
    QMatrix4x4 modelTranslation;
    QMatrix4x4 modelTransf;
    QMatrix4x4 toolRotation;
    QMatrix4x4 toolRotation2;
    QMatrix4x4 toolTranslation;
    QMatrix4x4 toolTranslation2;
    QMatrix4x4 toolTransf; // toolTranslation * modelRotation * modelScaling
    QMatrix4x4 toolTransf2;

    // Transformation matrix for the projection
    QMatrix4x4 projTransf;

    GLint modelLocation;
    GLint projLocation;

public:
    MainView(QWidget *parent = nullptr);
    ~MainView() override;

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(float scale);
    void setTime(float time);
    void setA(float a);
    void updateToolTransf();
    void updateAdjToolTransf();
    void updateBuffers();

protected:
    void initializeGL() override;
    void initBuffers();
    void resizeGL(int newWidth, int newHeight) override;
    void paintGL() override;
    void moveModel(float x, float y);
    QVector2D toNormalizedScreenCoordinates(float x, float y);

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *ev) override;

private slots:
    void onMessageLogged(QOpenGLDebugMessage Message);

private:
    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLFunctions_4_1_Core *gl;
    Settings settings;
};

#endif // MAINVIEW_H
