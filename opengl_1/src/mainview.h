#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

#include "model.h"
#include "vertex.h"
#include "cylinder.h"
#include "cylindermovement.h"
#include "simplepath.h"


/**
 * @brief The MainView class is resonsible for the actual content of the main
 * window.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
    friend class MainWindow;

    // Cylinder rendering
    Cylinder cylinder;
    GLuint vaoCyl;
    GLuint vboCyl;
    QVector<Vertex> vertexArrCyl;
    QOpenGLShaderProgram shader;

    // Path rendering
    CylinderMovement move;
    QVector<QVector3D> axisDirections;
    SimplePath path;
    GLuint vaoPth;
    GLuint vboPth;
    QVector<Vertex> vertexArrPth;

    // Transformation matrices for the model
    QMatrix4x4 modelScaling;
    QMatrix4x4 modelRotation;
    QMatrix4x4 cylinderRotation;
    QMatrix4x4 cylinderTranslation;
    QMatrix4x4 modelTranslation;
    QMatrix4x4 cylinderTransf; // cylinderTranslation * modelRotation * modelScaling
    QMatrix4x4 modelTransf;

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
    void updateBuffers();

protected:
    void initializeGL() override;
    void initBuffers();
    void resizeGL(int newWidth, int newHeight) override;
    void paintGL() override;

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

    QOpenGLShaderProgram shaderProgram;
    float time;

    void createShaderProgram();
};

#endif // MAINVIEW_H
