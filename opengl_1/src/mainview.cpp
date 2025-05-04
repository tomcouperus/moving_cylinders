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

    indicesUsed.fill(true, 2);

    toolRenderers.reserve(2);
    toolRenderers.append(new ToolRenderer());
    toolRenderers.append(new ToolRenderer());

    // Define the vertices of the tools
    cylinders.reserve(2);
    cylinders.append(new Cylinder());
    cylinders.append(new Cylinder());
    cylinders[0]->initCylinder();
    cylinders[1]->initCylinder();

    drums.reserve(2);
    drums.append(new Drum());
    drums.append(new Drum());
    drums[0]->initDrum();
    drums[1]->initDrum();

    switch (settings.toolIdx) {
    case 0:
        toolRenderers[0]->setTool(cylinders[0]);
        toolRenderers[1]->setTool(cylinders[1]);
        break;
    case 1:
        toolRenderers[0]->setTool(drums[0]);
        toolRenderers[1]->setTool(drums[1]);
        break;
    default:
        break;
    }

    // Define path for the tool
    SimplePath path = SimplePath(Polynomial(0.0,0.0,1.0,0.0),
                                 Polynomial(0.0,1.0,0.0,0.0),
                                 Polynomial());
    path.initVertexArr();

    // Define orientation(s) of the tool
    movements.reserve(2);
    movements.append(nullptr);
    movements.append(nullptr);
    movements[0] = new CylinderMovement(path,
                                        //QVector3D(0.0,0.1,-1.0),
                                        QVector3D(0.0,1.0,0.0),
                                        QVector3D(0.0,1.0,0.0), *cylinders[0]);
    movements[1] = new CylinderMovement(path,
                                        //QVector3D(0.0,0.1,-1.0),
                                        QVector3D(0.0,1.0,0.0),
                                        QVector3D(0.0,1.0,0.0), *cylinders[1]);

    moveRenderers.reserve(2);
    moveRenderers.append(new MoveRenderer());
    moveRenderers.append(new MoveRenderer());
    moveRenderers[0]->setMovement(movements[0]);
    moveRenderers[1]->setMovement(movements[1]);

    // Define the vertices of the enveloping surface
    envelopes.reserve(2);
    envelopes.append(new Envelope(&settings, 0, movements[0], cylinders[0]));
    envelopes[0]->initEnvelope();

    envelopes.append(new Envelope(&settings, 1, movements[1], cylinders[1], envelopes[0]));
    envelopes[1]->initEnvelope();
    envelopes[1]->setActive(false);

    envelopeRenderers.reserve(2);
    envelopeRenderers.append(new EnvelopeRenderer());
    envelopeRenderers.append(new EnvelopeRenderer());
    envelopeRenderers[0]->setEnvelope(envelopes[0]);
    envelopeRenderers[1]->setEnvelope(envelopes[1]);

    // First transformation of the cylinder
    modelTranslation.setToIdentity();
    modelTranslation.translate(-2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    modelTransf = modelTranslation;
    updateToolTransf();

    // Pass initial transformations to the renderers;
    for (int i = 0; i < indicesUsed.size(); i++) {
        toolRenderers[i]->setModelTransf(modelTransf);
        envelopeRenderers[i]->setModelTransf(modelTransf);
        moveRenderers[i]->setModelTransf(modelTransf);
    }

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
    indicesUsed.clear();
    indicesUsed.squeeze();
    for (auto i : toolRenderers){
        delete i;
    }
    toolRenderers.clear();
    toolRenderers.squeeze();
    for (auto i : drums){
        delete i;
    }
    drums.clear();
    drums.squeeze();
    for (auto i : cylinders){
        delete i;
    }
    cylinders.clear();
    cylinders.squeeze();
    for (auto i : movements){
        delete i;
    }
    movements.clear();
    movements.squeeze();
    for (auto i : moveRenderers){
        delete i;
    }
    moveRenderers.clear();
    moveRenderers.squeeze();
    for (auto i : envelopes){
        delete i;
    }
    envelopes.clear();
    envelopes.squeeze();
    for (auto i : envelopeRenderers){
        delete i;
    }
    envelopeRenderers.clear();
    envelopeRenderers.squeeze();

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
    for (int i = 0; i < indicesUsed.size(); i++) {
        toolRenderers[i]->init(gl,&settings);
        moveRenderers[i]->init(gl,&settings);
        envelopeRenderers[i]->init(gl,&settings);
    }
    updateBuffers();
    updateUniformsRequired = true;
}


/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    qDebug() << "main update buffers";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->updateBuffers();
        envelopeRenderers[i]->updateBuffers();
        moveRenderers[i]->updateBuffers();
    }
}

void MainView::updateUniforms() {
    qDebug() << "main update uniforms";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->updateUniforms();
        moveRenderers[i]->updateUniforms();
        envelopeRenderers[i]->updateUniforms();
    }
}


/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL()
{
    // Clear the screen before rendering
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (updateUniformsRequired) {
        updateUniforms();
        updateUniformsRequired = false;
    }

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->paintGL();
        moveRenderers[i]->paintGL();
        envelopeRenderers[i]->paintGL();
    }
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    qDebug() << "MainView::resizeGL";
    // Get the aspect ratio of the new screen size
    float aspectRatio = newWidth / ((float)newHeight);

    // Set the viewport to the new size
    projTransf.setToIdentity();
    projTransf.perspective(60.0f, aspectRatio, 0.2f, 20.0f);

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->setProjTransf(projTransf);
        envelopeRenderers[i]->setProjTransf(projTransf);
        moveRenderers[i]->setProjTransf(projTransf);
    }
    updateUniformsRequired = true;
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

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->setModelTransf(modelTransf);
        envelopeRenderers[i]->setModelTransf(modelTransf);
        moveRenderers[i]->setModelTransf(modelTransf);
    }
    updateToolTransf();
    updateUniformsRequired = true;
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

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->setModelTransf(modelTransf);
        envelopeRenderers[i]->setModelTransf(modelTransf);
        moveRenderers[i]->setModelTransf(modelTransf);
    }
    updateToolTransf();
    updateUniformsRequired = true;
    update();
}

/**
 * @brief MainView::setTime Changes the time of the displayed objects.
 * @param time The new time value.
 */
void MainView::setTime(float time)
{
    qDebug() << "Time changed to " << time;

    settings.time = time + movements[0]->getPath().getT0();

    updateToolTransf();
    updateUniformsRequired = true;
    update();
}

/**
 * @brief MainView::setA Changes the position value a of the displayed objects.
 * @param a The new a value.
 */
void MainView::setA(float a)
{
    float divisor;
    switch (settings.toolIdx) {
    case 0:
        divisor = cylinders[0]->getSectors() / (cylinders[0]->getA1() - cylinders[0]->getA0());
        break;
    case 1:
        divisor = drums[0]->getSectors() / (drums[0]->getA1() - drums[0]->getA0());
        break;
    default:
        break;
    }

    settings.a = a / divisor;
    updateUniformsRequired = true;
    update();
}

/**
 * @brief MainView::updateToolTransf Updates the tools transformation matrices.
 */
void MainView::updateToolTransf(){
    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->setToolTransf(envelopes[i]->getToolTransform());
    }
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
