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

    // Set model translation
    modelTranslation.translate(-2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    modelTransf = modelTranslation;

    // Initial envelopes
    Envelope *env0 = addNewEnvelope_noInit();
    Envelope *env1 = addNewEnvelope_noInit();
    env1->setAdjacentEnvelope(env0);
    env1->update();
    addNewEnvelope_noInit();

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

/**
 * @brief MainView::addNewEnvelope_noInit Creates a new envelope but does not initialize the renderers.
 * Call MainView::initRenderers(Envelope *) for that.
 * Separation required due to order of initialization of MainWindow and openGL.
 * @return
 */
Envelope* MainView::addNewEnvelope_noInit() {
    // Mark a new index as used.
    // TODO replace with pooling system to avoid repeatedly allocating and freeing memory,
    // though might not have that big of an impact, the option is there.
    int idx = indicesUsed.size();
    indicesUsed.append(true);

    // Tools
    Cylinder *cyl = new Cylinder();
    cyl->initCylinder();
    cylinders.append(cyl);
    Drum *drum = new Drum();
    drum->initDrum();
    drums.append(drum);
    ToolRenderer *toolRend = new ToolRenderer();
    toolRend->setTool(cyl);
    toolRend->setModelTransf(modelTransf);
    toolRend->setProjTransf(projTransf);
    toolRenderers.append(toolRend);

    // Path and envelope
    SimplePath path = SimplePath(Polynomial(0,0,0,0),
                                 Polynomial(0,0,0,0),
                                 Polynomial(0,0,1,0));
    Envelope *env = new Envelope(idx, cyl, path);
    env->initEnvelope();
    envelopes.append(env);
    EnvelopeRenderer *envRend = new EnvelopeRenderer(env);
    envRend->setModelTransf(modelTransf);
    envRend->setProjTransf(projTransf);
    envelopeRenderers.append(envRend);

    // Move Renderer
    MoveRenderer *moveRend = new MoveRenderer(&env->getToolMovement());
    moveRend->setModelTransf(modelTransf);
    moveRend->setProjTransf(projTransf);
    moveRenderers.append(moveRend);

    return env;
}

/**
 * @brief MainView::initRenderers Initializes the renderers for a given envelope.
 * Required due to order of initialization of MainWindow and openGL.
 * @param env
 */
void MainView::initRenderers(const Envelope *env) {
    int idx = env->getIndex();
    toolRenderers[idx]->init(gl, &settings);
    envelopeRenderers[idx]->init(gl, &settings);
    moveRenderers[idx]->init(gl, &settings);
    qDebug() << "Initialized renderers for envelope "+QString::number(env->getIndex());
}

/**
 * @brief MainView::addNewEnvelope Creates a new envelope
 * @return
 */
Envelope* MainView::addNewEnvelope() {
    Envelope *env = addNewEnvelope_noInit();
    initRenderers(env);
    return env;
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

    initRenderers(envelopes[0]);
    initRenderers(envelopes[1]);
    initRenderers(envelopes[2]);

    // Trigger buffer update
    updateBuffers();
    updateToolTransf();
    updateAllUniforms = true;
}


/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    qDebug() << "main update buffers";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->updateBuffers();
        envelopeRenderers[i]->updateBuffers();
        moveRenderers[i]->updateBuffers();
    }
}

void MainView::updateUniforms() {
    qDebug() << "main update uniforms";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
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

    if (!envelopeMeshUpdates.isEmpty()) {
        QList<int> indices = envelopeMeshUpdates.values();
        while (!indices.isEmpty()) {
            int i = indices.takeFirst();
            envelopes[i]->update();
            envelopeRenderers[i]->updateBuffers();
        }
        envelopeMeshUpdates.clear();
    }

    if (!toolMeshUpdates.isEmpty()) {
        QList<int> indices = toolMeshUpdates.values();
        while (!indices.isEmpty()) {
            int i = indices.takeFirst();
            cylinders[i]->update();
            drums[i]->update();
            toolRenderers[i]->updateBuffers();
        }
    }

    if (!toolTransfUpdates.isEmpty()) {
        QList<int> indices = toolTransfUpdates.values();
        while (!indices.isEmpty()) {
            int i = indices.takeFirst();
            toolRenderers[i]->setToolTransf(envelopes[i]->getToolTransformAt(settings.t()));
            toolRenderers[i]->updateUniforms();
        }
        toolTransfUpdates.clear();
    }

    if (updateAllUniforms) {
        updateUniforms();
        updateAllUniforms = false;
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
    updateAllUniforms = true;
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
    updateAllUniforms = true;
    update();
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
// TODO remove the update call here and move it to wherever in mainwindow this function is called
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
    updateAllUniforms = true;
    update();
}

/**
 * @brief MainView::updateToolTransf Updates the tools transformation matrices.
 */
void MainView::updateToolTransf(){
    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->setToolTransf(envelopes[i]->getToolTransformAt(settings.t()));
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
