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
 * @brief MainView::addNewEnvelope Creates a new envelope. Return nullptr if no new envelope can be created.
 * @return
 */
Envelope* MainView::addNewEnvelope() {
    int idx = envelopes.indexOf(nullptr);
    if (idx == -1) return nullptr;

    // Tools
    Cylinder *cyl = new Cylinder();
    cyl->initCylinder();
    cylinders[idx] = cyl;
    Drum *drum = new Drum();
    drum->initDrum();
    drums[idx] = drum;

    // Path and envelope
    SimplePath path = SimplePath(Polynomial(0,0,0,0),
                                 Polynomial(0,0,0,0),
                                 Polynomial(0,0,1,0));
    Envelope *env = new Envelope(idx, cyl, path);
    env->initEnvelope();
    envelopes[idx] = env;

    // Set related renderers
    toolRenderers[idx]->setTool(cyl);
    toolRenderers[idx]->setModelTransf(modelTransf);
    toolRenderers[idx]->setProjTransf(projTransf);

    envelopeRenderers[idx]->setEnvelope(env);
    envelopeRenderers[idx]->setModelTransf(modelTransf);
    envelopeRenderers[idx]->setProjTransf(projTransf);

    moveRenderers[idx]->setMovement(&env->getToolMovement());
    moveRenderers[idx]->setModelTransf(modelTransf);
    moveRenderers[idx]->setProjTransf(projTransf);

    // Activate
    indicesUsed[idx] = true;
    return env;
}

/**
 * @brief MainView::deleteEnvelope. Deletes an envelope. The pointer will point to nothing.
 * @param env
 */
void MainView::deleteEnvelope(Envelope *env) {
    qDebug() << "TODO";
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

    // Set model translation
    modelTranslation.translate(-2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    modelTransf = modelTranslation;

    /***********************************************************/
    /************************ IMPORTANT ************************/
    /***********************************************************/
    // Initialize the VAOs and VBOs of a pool of renderers.
    // This pool is used due to an unknown bug that on Windows systems assigns
    // used VAO memory addresses when initializing them outside of this function.
    // It is likely an issue with a stray openGL command messing something up,
    // but due to time constraints and the scope of the code that needs investigating
    // it is not yet solved. The pool, though imposing restrictions on the amount of envelopes,
    // was an acceptable compromise at the time.
    // Changing this will also require reworking the addNewEnvelope and removeEnvelope functions.
    indicesUsed.fill(false, settings.NUM_ENVELOPES);
    toolRenderers.reserve(settings.NUM_ENVELOPES);
    envelopeRenderers.reserve(settings.NUM_ENVELOPES);
    moveRenderers.reserve(settings.NUM_ENVELOPES);
    envelopes.fill(nullptr, settings.NUM_ENVELOPES);
    cylinders.fill(nullptr, settings.NUM_ENVELOPES);
    drums.fill(nullptr, settings.NUM_ENVELOPES);
    for (size_t i = 0; i < settings.NUM_ENVELOPES; i++) {
        ToolRenderer *toolRend = new ToolRenderer();
        toolRend->init(gl, &settings);
        toolRenderers.append(toolRend);

        EnvelopeRenderer *envRend = new EnvelopeRenderer();
        envRend->init(gl, &settings);
        envelopeRenderers.append(envRend);

        MoveRenderer *moveRend = new MoveRenderer();
        moveRend->init(gl, &settings);
        moveRenderers.append(moveRend);
    }

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
        toolMeshUpdates.clear();
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
