#include <math.h>
#include "mainview.h"
#include "vertex.h"

#include <QDateTime>
#include <QOpenGLVersionFunctionsFactory>

/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView()
{
    qDebug() << "MainView destructor";

    makeCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL()
{
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
            SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize())
    {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    // glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // grab the opengl context
    gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(
            this->context());

    // Set the color to be used by glClear.
    // This is the background color.
    glClearColor(0.37f, 0.42f, 0.45f, 0.0f);

    // TODO: refactor

    // Define the vertices of the tools
    cylinder.initCylinder();
    qDebug() << "a0:" << cylinder.getA0() << "a1:" << cylinder.getA1() << "H:" << cylinder.getHeight();
    drum.initDrum();
    qDebug() << "a0:" << drum.getA0() << "a1:" << drum.getA1() << "H:" << drum.getHeight();

    switch (settings.toolIdx) {
    case 0:
        toolRend.setTool(&cylinder);
        toolRend2.setTool(&cylinder);
        break;
    case 1:
        toolRend.setTool(&drum);
        toolRend2.setTool(&drum);
        break;
    default:
        break;
    }
    toolRend.init(gl,&settings);
    toolRend2.init(gl,&settings);

    // Define path for the tool
    SimplePath path = SimplePath(Polynomial(0.0,0.0,1.0,0.0),
                      Polynomial(0.0,1.0,0.0,0.0),
                      Polynomial());
    path.initVertexArr();
    // Define orientation(s) of the tool
    move = CylinderMovement(path,
                            QVector3D(0.0,0.1,-1.0),
                            QVector3D(0.0,1.0,0.0), cylinder);
    move2 = CylinderMovement(path,
                            QVector3D(0.0,0.1,-1.0),
                            QVector3D(0.0,1.0,0.0), cylinder);

    movRend.setMovement(&move);
    movRend.init(gl,&settings);
    movRend2.setMovement(&move2);
    movRend2.init(gl,&settings);

    // Define the vertices of the enveloping surface
    envelope = Envelope(move, &cylinder);
    envelope.initEnvelope();

    qDebug() << "env2";
    envelope2 = Envelope(move2, &cylinder, &envelope);
    envelope2.initEnvelope();

    envRend.setEnvelope(&envelope);
    envRend.init(gl,&settings);
    envRend2.setEnvelope(&envelope2);
    envRend2.init(gl,&settings);

    initBuffers();

    // First transformation of the cylinder
    toolTranslation.setToIdentity();
    toolTranslation.translate(-2, 0, -6);
    QVector3D tool2Posit = envelope.getEndCurveArrAt(0);
    toolTranslation2 = toolTranslation;
    toolTranslation2.translate(tool2Posit);
    modelTranslation = toolTranslation;

    toolRotation.setToIdentity();
    toolRotation = move.getMovementRotation(0);

    toolRotation2.setToIdentity();
    toolRotation2 = move2.getMovementRotation(0);

    // Set the initial model transformation to
    // just the translation
    toolTransf = toolTranslation * toolRotation;
    toolTransf2 = toolTranslation2 * toolRotation2;
    modelTransf = modelTranslation;

    // Pass initial transformations to the renderers;
    toolRend.setTransf(toolTransf);
    toolRend2.setTransf(toolTransf2);
    envRend.setTransf(modelTransf);
    envRend2.setTransf(modelTransf);
    movRend.setTransf(modelTransf);
    movRend2.setTransf(modelTransf);

    // Set the initial projection transformation
    projTransf.setToIdentity();
    projTransf.ortho(0.0f,20.0f,0.0f,20.0f,0.2f,20.0f);
}

/**
 * @brief MainView::initBuffers Initialises the buffers
 * TODO: extend for other cylinders and enveloping surfaces.
 */
void MainView::initBuffers() {
    toolRend.initBuffers();
    toolRend2.initBuffers();

    movRend.initBuffers();
    movRend2.initBuffers();

    envRend.initBuffers();
    envRend2.initBuffers();
}

/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    qDebug() << "main update buffers";
    switch (settings.toolIdx) {
    case 0:
        envelope.setTool(&cylinder);
        envelope2.setAdjacentEnvelope(&envelope);
        envelope2.setTool(&cylinder);
        toolRend.updateBuffers(&cylinder);
        toolRend2.updateBuffers(&cylinder);
        break;
    case 1:
        qDebug() << "is drum";
        envelope.setTool(&drum);
        envelope2.setAdjacentEnvelope(&envelope);
        envelope2.setTool(&drum);
        toolRend.updateBuffers(&drum);
        toolRend2.updateBuffers(&drum);
        break;
    default:
        break;
    }
    envRend.updateBuffers(&envelope);
    envRend2.updateBuffers(&envelope2);

    movRend.updateBuffers(&move);
    movRend2.updateBuffers(&move2);
}


/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL()
{
    // Clear the screen before rendering
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the shader program

    toolRend.updateUniforms(toolTransf, projTransf);
    toolRend.paintGL();
    toolRend2.updateUniforms(toolTransf, projTransf);
    toolRend2.paintGL();

    movRend.updateUniforms(modelTransf,projTransf);
    movRend.paintGL();
    movRend2.updateUniforms(modelTransf,projTransf);
    movRend2.paintGL();

    envRend.updateUniforms(modelTransf,projTransf);
    envRend.paintGL();
    envRend2.updateUniforms(modelTransf,projTransf);
    envRend2.paintGL();
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    // Get the aspect ratio of the new screen size
    float aspectRatio = newWidth / ((float)newHeight);

    // Set the viewport to the new size
    projTransf.setToIdentity();
    projTransf.perspective(60.0f, aspectRatio, 0.2f, 20.0f);
}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << ","
             << rotateZ << ")";

    // Reset the rotation matrix
    modelRotation.setToIdentity();

    // Make the new rotation matrix
    modelRotation.rotate(rotateX, 1, 0, 0); // rotate around x axis
    modelRotation.rotate(rotateY, 0, 1, 0); // rotate around y axis
    modelRotation.rotate(rotateZ, 0, 0, 1); // rotate around z axis

    // Update the model transformation matrix
    modelTransf = modelTranslation * modelScaling * modelRotation;

    envRend.setTransf(modelTransf);
    envRend2.setTransf(modelTransf);
    movRend.setTransf(modelTransf);
    movRend2.setTransf(modelTransf);
    updateToolTransf();
    update();
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float scale)
{
    qDebug() << "Scale changed to " << scale;

    // Reset the scale matrix
    modelScaling.setToIdentity();

    // Make the new scale matrix
    modelScaling.scale(scale);

    // Update the model transformation matrix
    modelTransf = modelTranslation * modelScaling * modelRotation;

    envRend.setTransf(modelTransf);
    envRend2.setTransf(modelTransf);
    movRend.setTransf(modelTransf);
    movRend2.setTransf(modelTransf);
    updateToolTransf();
    update();
}

/**
 * @brief MainView::setTime Changes the time of the displayed objects.
 * @param time The new time value.
 */
void MainView::setTime(float time)
{
    qDebug() << "Time changed to " << time;

    settings.time = time + move.getPath().getT0();

    updateToolTransf();
    update();
}

void MainView::setA(float a)
{
    float divisor;
    switch (settings.toolIdx) {
    case 0:
        divisor = cylinder.getSectors() / (cylinder.getA1() - cylinder.getA0());
        break;
    case 1:
        divisor = drum.getSectors() / (drum.getA1() - drum.getA0());
        break;
    default:
        break;
    }

    settings.a = a / divisor;
}

void MainView::updateToolTransf(){
    SimplePath path = move.getPath();
    toolTranslation.setToIdentity();
    toolTranslation = modelTranslation;
    QVector4D shift = QVector4D(envelope.getPathAt(settings.time),0);
    shift = modelTransf * shift;
    toolTranslation.translate(QVector3D(shift.x(),shift.y(),shift.z()));

    toolTranslation2.setToIdentity();
    toolTranslation2 = modelTranslation;
    QVector4D shift2 = QVector4D(envelope2.getPathAt(settings.time),0);
    shift2 = modelTransf * shift2;
    toolTranslation2.translate(QVector3D(shift2.x(),shift2.y(),shift2.z()));

    toolRotation.setToIdentity();
    toolRotation = move.getMovementRotation(settings.time);
    toolRotation2.setToIdentity();
    toolRotation2 = move2.getMovementRotation(settings.time);

    // Update the model transformation matrix
    toolTransf = toolTranslation * modelScaling * modelRotation * toolRotation;


    toolTransf2 = toolTranslation2 * modelScaling * modelRotation * toolRotation2;

    //toolRend.updateUniforms(toolTransf, projTransf);
    toolRend.setTransf(toolTransf);
    toolRend2.setTransf(toolTransf2);
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message)
{
    qDebug() << " → Log:" << Message;
}
