#include "renderer.h"

/**
 * @brief Renderer::Renderer Creates a new renderer.
 */
Renderer::Renderer() : gl(nullptr) {}

/**
 * @brief Renderer::~Renderer Deconstructs the renderer by deleting all shaders.
 */
Renderer::~Renderer() {}

/**
 * @brief Renderer::init Initialises the renderer with an OpenGL context and
 * settings. Also initialises the shaders and buffers.
 * @param f OpenGL functions pointer.
 * @param s Settings.
 */
void Renderer::init(QOpenGLFunctions_4_1_Core* f, Settings *s) {
    gl = f;
    settings = s;

    initShaders();
    initBuffers();
}
