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
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &knotVbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &knotVao);

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

    // TODO: refactor, delete knot

    // Define the vertices of the cylinder
    cylinder.initCylinder();
    vertexArr = cylinder.getVertexArr();

    // Load the knot model
    knotVertices = Model(":/models/knot.obj").getMeshCoords();

    for (int i = 0; i < knotVertices.size(); i++)
    {
        float r = fabs(knotVertices[i].x());
        float g = fabs(knotVertices[i].y());
        float b = fabs(knotVertices[i].z());

        knotArr.append(
            Vertex(
                knotVertices[i].x(),
                knotVertices[i].y(),
                knotVertices[i].z(),
                r, g, b));
    }

    // Make a VAO for the pyramid
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    updateBuffers();

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

    // Make a VAO for the knot
    glGenVertexArrays(1, &knotVao);
    glBindVertexArray(knotVao);

    glGenBuffers(1, &knotVbo);
    glBindBuffer(GL_ARRAY_BUFFER, knotVbo);
    glBufferData(GL_ARRAY_BUFFER, knotArr.size() * sizeof(Vertex), knotArr.data(), GL_STATIC_DRAW);

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

    // First transformation of the pyramid
    pyramidTranslation.setToIdentity();
    pyramidTranslation.translate(-2, 0, -6);

    // First transformation of the knot
    knotTranslation.setToIdentity();
    knotTranslation.translate(2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    pyramidTransf = pyramidTranslation;
    knotTransf = knotTranslation;

    // Set the initial projection transformation
    projTransf.setToIdentity();
    projTransf.perspective(60.0f, 1.0f, 0.2f, 20.0f);
}

/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    vertexArr.clear();
    vertexArr = cylinder.getVertexArr();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexArr.size() * sizeof(Vertex), vertexArr.data(), GL_STATIC_DRAW);
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
    // Set the pyramid model
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, pyramidTransf.data());
    // Bind the vao
    glBindVertexArray(vao);
    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, vertexArr.size());
    shaderProgram.release();

    // Bind the shader program
    shaderProgram.bind();
    // Set the knot model
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, knotTransf.data());
    // Bind the vao
    glBindVertexArray(knotVao);
    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, knotArr.size());
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
    knotTransf = knotTranslation * modelScaling * modelRotation;
    pyramidTransf = pyramidTranslation * modelScaling * modelRotation;
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
    knotTransf = knotTranslation * modelScaling * modelRotation;
    pyramidTransf = pyramidTranslation * modelScaling * modelRotation;
    update();
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
