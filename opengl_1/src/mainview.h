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

    // Administration
    QVector<bool> indicesUsed;
    // NOTE: We do not delete any of pointers in the arrays below to avoid creating and deleting objects.
    // If the pool needs to increase, we do so, and keep track in the indicesUsed array for which objects are in use.
    // There are likely optimizations possible to shrink memory usage when possible, but not for now.
    bool updateUniformsRequired;

    // Tool rendering
    QVector<ToolRenderer*> toolRenderers;
    QVector<Drum*> drums;
    QVector<Cylinder*> cylinders;

    // Path rendering
    QVector<CylinderMovement*> movements;
    QVector<MoveRenderer*> moveRenderers;

    // Envelope rendering
    QVector<Envelope*> envelopes;
    QVector<EnvelopeRenderer*> envelopeRenderers;

    // Transformation matrices for the model
    QMatrix4x4 modelScaling;
    QMatrix4x4 modelRotation;
    QMatrix4x4 modelTranslation;
    QMatrix4x4 modelTransf;
    QVector<QMatrix4x4*> toolRotations;
    QVector<QMatrix4x4*> toolToPathTranslations;
    QVector<QMatrix4x4*> toolTranslations;
    QVector<QMatrix4x4*> toolTransforms;

    // Transformation matrix for the projection
    QMatrix4x4 projTransf;

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
    void updateUniforms();
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
