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
    toolRenderers[0]->init(gl,&settings);
    toolRenderers[1]->init(gl,&settings);

    // Define path for the tool
    SimplePath path = SimplePath(Polynomial(0.0,0.0,1.0,0.0),
                      Polynomial(0.0,1.0,0.0,0.0),
                      Polynomial());
    path.initVertexArr();

    // Define orientation(s) of the tool
    move = CylinderMovement(path,
                            //QVector3D(0.0,0.1,-1.0),
                            QVector3D(0.0,1.0,0.0),
                            QVector3D(0.0,1.0,0.0), *cylinders[0]);
    move2 = CylinderMovement(path,
                            //QVector3D(0.0,0.1,-1.0),
                            QVector3D(0.0,1.0,0.0),
                            QVector3D(0.0,1.0,0.0), *cylinders[1]);

    movRend.setMovement(&move);
    movRend.init(gl,&settings);
    movRend2.setMovement(&move2);
    movRend2.init(gl,&settings);

    // Define the vertices of the enveloping surface
    envelope = Envelope(&move, cylinders[0]);
    envelope.initEnvelope();

    envelope2 = Envelope(&move2, cylinders[1], &envelope);
    envelope2.initEnvelope();

    envRend.setEnvelope(&envelope);
    envRend.init(gl,&settings);
    envRend2.setEnvelope(&envelope2);
    envRend2.init(gl,&settings);

    initBuffers();

    // First transformation of the cylinder
    modelTranslation.setToIdentity();
    modelTranslation.translate(-2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    modelTransf = modelTranslation;
    updateToolTransf();

    // Pass initial transformations to the renderers;
    toolRenderers[0]->setTransf(toolTransf);
    toolRenderers[1]->setTransf(toolTransf2);
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
    toolRenderers[0]->initBuffers();
    toolRenderers[1]->initBuffers();

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
        envelope.setTool(cylinders[0]);
        envelope2.setAdjacentEnvelope(&envelope);
        toolRenderers[0]->updateBuffers(cylinders[0]);
        break;
    case 1:
        qDebug() << "is drum";
        envelope.setTool(drums[0]);
        envelope2.setAdjacentEnvelope(&envelope);
        toolRenderers[0]->updateBuffers(drums[0]);
        break;
    default:
        break;
    }
    envRend.updateBuffers(&envelope);
    movRend.updateBuffers(&move);

    if (settings.secondEnv) {
        switch (settings.tool2Idx) {
        case 0:
            envelope2.setTool(cylinders[1]);
            toolRenderers[1]->updateBuffers(cylinders[1]);
            break;
        case 1:
            qDebug() << "is drum";
            envelope2.setTool(drums[1]);
            toolRenderers[1]->updateBuffers(drums[1]);
            break;
        default:
            break;
        }
        envRend2.updateBuffers(&envelope2);
        movRend2.updateBuffers(&move2);
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

    // Bind the shader program

    toolRenderers[0]->updateUniforms(toolTransf, projTransf);
    toolRenderers[0]->paintGL();

    movRend.updateUniforms(modelTransf,projTransf);
    movRend.paintGL();

    envRend.updateUniforms(modelTransf,projTransf);
    envRend.paintGL();

    if (settings.secondEnv) {
        toolRenderers[1]->updateUniforms(toolTransf, projTransf);
        toolRenderers[1]->paintGL();

        movRend2.updateUniforms(modelTransf,projTransf);
        movRend2.paintGL();

        envRend2.updateUniforms(modelTransf,projTransf);
        envRend2.paintGL();
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
}

/**
 * @brief MainView::updateToolTransf Updates the tools transformation matrices.
 */
void MainView::updateToolTransf(){
    // tool translation towards path
    toolToPathTranslation.setToIdentity();
    QVector3D toolPosit = -envelope.getTool()->getA0()*envelope.getTool()->getAxisVector();
    QVector4D shift = QVector4D(toolPosit,0);
    toolToPathTranslation.translate(QVector3D(shift.x(),shift.y(),shift.z()));

    // tool translation w.r.t path
    toolTranslation.setToIdentity();
    toolPosit = envelope.getPathAt(settings.time);
    shift = QVector4D(toolPosit,0);
    toolTranslation.translate(QVector3D(shift.x(),shift.y(),shift.z()));

    toolRotation.setToIdentity();
    toolRotation = move.getMovementRotation(settings.time);

    // Update the model transformation matrix
    // since the rotation is centered on the path which lies higher on the tool axis we have to translate the tool first
    toolTransf = modelTransf * toolTranslation * toolRotation * toolToPathTranslation;

    toolRenderers[0]->updateUniforms(toolTransf, projTransf);
    toolRenderers[0]->setTransf(toolTransf);

    if (settings.secondEnv) {
        updateAdjToolTransf();
    }
}

/**
 * @brief MainView::updateAdjToolTransf Updates the tools transformation matrices of a second tool.
 */
void MainView::updateAdjToolTransf(){
    // tool translation towards path
    toolToPathTranslation2.setToIdentity();
    QVector3D toolPosit = -envelope2.getTool()->getA0()*envelope2.getTool()->getAxisVector();
    QVector4D shift2 = QVector4D(toolPosit,0);
    toolToPathTranslation2.translate(QVector3D(shift2.x(),shift2.y(),shift2.z()));

    toolTranslation2.setToIdentity();
    toolPosit = envelope2.getPathAt(settings.time);
    shift2 = QVector4D(toolPosit,0);
    toolTranslation2.translate(QVector3D(shift2.x(),shift2.y(),shift2.z()));

    toolRotation2.setToIdentity();
    if (envelope2.isTanContinuous()) {
        toolRotation2 = envelope2.getAdjMovementRotation(settings.time);

        // Update the model transformation matrix
        // since the rotation is centered on the path which lies higher on the tool axis we have to translate the tool first
        toolTransf2 = modelTransf * toolTranslation2 * toolRotation2 * toolRotation * toolToPathTranslation2;
    } else {
        toolRotation2 = move2.getMovementRotation(settings.time);

        // Update the model transformation matrix
        toolTransf2 = modelTransf * toolTranslation2 * toolRotation2 * toolToPathTranslation2;
    }

    toolRenderers[1]->updateUniforms(toolTransf, projTransf);
    toolRenderers[1]->setTransf(toolTransf2);
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
