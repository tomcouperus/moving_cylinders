#include <math.h>
#include "mainview.h"
#include "vertex.h"

#include <QDateTime>

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

    // CLEAN UP!
    glDeleteBuffers(1, &vboTool);
    glDeleteVertexArrays(1, &vaoTool);
    glDeleteBuffers(1, &vboPth);
    glDeleteVertexArrays(1, &vaoPth);
    glDeleteBuffers(1, &vboEnv);
    glDeleteVertexArrays(1, &vaoEnv);
    glDeleteBuffers(1, &vboCenters);
    glDeleteVertexArrays(1, &vaoCenters);
    glDeleteBuffers(1, &vboGrazingCurve);
    glDeleteVertexArrays(1, &vaoGrazingCurve);

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

    // Set the color to be used by glClear.
    // This is the background color.
    glClearColor(0.37f, 0.42f, 0.45f, 0.0f);

    createShaderProgram();

    // TODO: refactor

    // Define the vertices of the cylinder
    cylinder.initCylinder();
    vertexArrTool = cylinder.getVertexArr();
    qDebug() << "a0:" << cylinder.getA0() << "a1:" << cylinder.getA1() << "H:" << cylinder.getHeight();

    // Initialize the drum
    drum.initDrum();
    qDebug() << "a0:" << drum.getA0() << "a1:" << drum.getA1() << "H:" << drum.getHeight();


    SimplePath path = SimplePath(Polynomial(0.0,0.0,1.0,0.0),
                      Polynomial(0.0,1.0,0.0,0.0),
                      Polynomial());
    path.initVertexArr();
    vertexArrPth = path.getVertexArr();

    move = CylinderMovement(path,
                            QVector3D(0.0,0.1,-1.0),
                            QVector3D(0.0,1.0,0.0), cylinder);
    axisDirections = move.getAxisDirections();

    // Define the vertices of the enveloping surface
    envelope = Envelope(move, &cylinder);
    envelope.initEnvelope();
    vertexArrEnv = envelope.getVertexArr();
    vertexArrCenters = envelope.getVertexArrCenters();
    vertexArrGrazingCurve = envelope.getVertexArrGrazingCurve();
    qDebug() << "size" << vertexArrEnv.size();

    initBuffers();

    // First transformation of the cylinder
    toolTranslation.setToIdentity();
    toolTranslation.translate(-2, 0, -6);
    modelTranslation = toolTranslation;

    toolRotation.setToIdentity();
    toolRotation = move.getMovementRotation(0);

    // Set the initial model transformation to
    // just the translation
    toolTransf = toolTranslation * toolRotation;
    modelTransf = modelTranslation;

    // Set the initial projection transformation
    projTransf.setToIdentity();
    projTransf.ortho(0.0f,20.0f,0.0f,20.0f,0.2f,20.0f);
    //projTransf.perspective(60.0f, 1.0f, 0.2f, 20.0f);
}

/**
 * @brief MainView::initBuffers Initialises the buffers
 * TODO: extend for other cylinders and enveloping surfaces.
 */
void MainView::initBuffers() {
    glGenVertexArrays(1, &vaoTool);
    glBindVertexArray(vaoTool);
    glGenBuffers(1, &vboTool);

    glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        0,                               // index
        3,                               // size of part of interest (x,y,z)
        GL_FLOAT,                        // type
        GL_FALSE,                        // normalized
        sizeof(Vertex),                  // stride (size of object)
        (void *)offsetof(Vertex, xCoord) // offset
        );
    glVertexAttribPointer(
        1,                             // index
        3,                             // size of part of interest (r,g,b)
        GL_FLOAT,                      // type
        GL_FALSE,                      // normalized
        sizeof(Vertex),                // stride (size of object)
        (void *)offsetof(Vertex, rVal) // offset
        );

    glGenVertexArrays(1, &vaoPth);
    glBindVertexArray(vaoPth);
    glGenBuffers(1, &vboPth);

    glBindBuffer(GL_ARRAY_BUFFER, vboPth);
    glBufferData(GL_ARRAY_BUFFER, vertexArrPth.size() * sizeof(Vertex), vertexArrPth.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    glGenVertexArrays(1, &vaoEnv);
    glBindVertexArray(vaoEnv);
    glGenBuffers(1, &vboEnv);

    glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
    glBufferData(GL_ARRAY_BUFFER, vertexArrEnv.size() * sizeof(Vertex), vertexArrEnv.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    glGenVertexArrays(1, &vaoCenters);
    glBindVertexArray(vaoCenters);
    glGenBuffers(1, &vboCenters);

    glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
    glBufferData(GL_ARRAY_BUFFER, vertexArrCenters.size() * sizeof(Vertex), vertexArrCenters.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    glGenVertexArrays(1, &vaoGrazingCurve);
    glBindVertexArray(vaoGrazingCurve);
    glGenBuffers(1, &vboGrazingCurve);

    glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
    glBufferData(GL_ARRAY_BUFFER, vertexArrGrazingCurve.size() * sizeof(Vertex), vertexArrGrazingCurve.data(), GL_STATIC_DRAW);

    // Set up the vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
}

/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    vertexArrTool.clear();
    if(settings.isCylinder){
        vertexArrTool = cylinder.getVertexArr();

        envelope = Envelope(move, &cylinder);
    } else {
        qDebug() << "is drum";
        vertexArrTool = drum.getVertexArr();

        envelope = Envelope(move, &drum);
    }

    envelope.initEnvelope();
    vertexArrEnv.clear();
    vertexArrEnv = envelope.getVertexArr();

    vertexArrPth.clear();
    vertexArrPth = move.getPath().getVertexArr();

    vertexArrCenters.clear();
    vertexArrCenters = envelope.getVertexArrCenters();

    vertexArrGrazingCurve.clear();
    vertexArrGrazingCurve = envelope.getVertexArrGrazingCurve();

    glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    glBufferData(GL_ARRAY_BUFFER, vertexArrTool.size() * sizeof(Vertex), vertexArrTool.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboPth);
    glBufferData(GL_ARRAY_BUFFER, vertexArrPth.size() * sizeof(Vertex), vertexArrPth.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
    glBufferData(GL_ARRAY_BUFFER, vertexArrEnv.size() * sizeof(Vertex), vertexArrEnv.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
    glBufferData(GL_ARRAY_BUFFER, vertexArrCenters.size() * sizeof(Vertex), vertexArrCenters.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
    glBufferData(GL_ARRAY_BUFFER, vertexArrGrazingCurve.size() * sizeof(Vertex), vertexArrGrazingCurve.data(), GL_STATIC_DRAW);
}

/**
 * @brief MainView::createShaderProgram Creates a new shader program with a
 * vertex and fragment shader.
 */
void MainView::createShaderProgram()
{
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/fragshader.glsl");
    shaderProgram.link();

    // Get the locations of the model and projection matrices
    modelLocation = shaderProgram.uniformLocation("modelTransform");
    projLocation = shaderProgram.uniformLocation("projTransform");
}

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL()
{
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the shader program
    shaderProgram.bind();
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, projTransf.data());

    // Bind cylinder buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboTool);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, toolTransf.data());
    glBindVertexArray(vaoTool);
    // Draw cylinder
    glDrawArrays(GL_TRIANGLES,0,vertexArrTool.size());

    if(settings.showPath){
        // Bind path buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboPth);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelTransf.data());
        glBindVertexArray(vaoPth);
        // Draw path
        glDrawArrays(GL_LINE_STRIP,0,vertexArrPth.size());
    }

    if(settings.showEnvelope){
        // Bind envelope buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboEnv);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelTransf.data());
        glBindVertexArray(vaoEnv);
        // Draw envelope
        glDrawArrays(GL_TRIANGLES,0,vertexArrEnv.size());
    }

    if(settings.showToolAxis){
        // Bind centers buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboCenters);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelTransf.data());
        glBindVertexArray(vaoCenters);
        // Draw centers
        glDrawArrays(GL_LINES,0,vertexArrCenters.size());
    }

    if(settings.showGrazingCurve){
        // Bind grazing curve buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboGrazingCurve);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelTransf.data());
        glBindVertexArray(vaoGrazingCurve);
        // Draw grazing curve
        glDrawArrays(GL_LINES,0,vertexArrGrazingCurve.size());
    }

    shaderProgram.release();

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

    this->time = time + move.getPath().getT0();

    updateToolTransf();
    update();
}


void MainView::updateToolTransf(){
    SimplePath path = move.getPath();
    toolTranslation.setToIdentity();
    toolTranslation = modelTranslation;
    QVector4D shift = QVector4D(path.getPathAt(time),0);
    shift = modelTransf * shift;
    toolTranslation.translate(QVector3D(shift.x(),shift.y(),shift.z()));

    toolRotation.setToIdentity();
    toolRotation = move.getMovementRotation(time);

    // Update the model transformation matrix
    toolTransf = toolTranslation * modelScaling * modelRotation * toolRotation;
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
