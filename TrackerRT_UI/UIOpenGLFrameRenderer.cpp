#include "UIOpenGLFrameRenderer.h"
#include "ui_UIOpenGLFrameRenderer.h"

#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../FastMemory/fast_mem_pool.h"
#include "GL/glu.h"

UIOpenGLFrameRenderer::UIOpenGLFrameRenderer(QWidget *parent):
    QGLWidget(parent)
{
    _width = 0;
    _height = 0;
    _texture = 0;
    _fps = 0;
}

UIOpenGLFrameRenderer::~UIOpenGLFrameRenderer()
{
    glDeleteTextures(1, &_texture);
}

void UIOpenGLFrameRenderer::_tick()
{
    // triggers paintGL()
    update();

    // Set timer according to FPS
    QTimer::singleShot(1000/_fps, this, SLOT(_tick()));
}

void UIOpenGLFrameRenderer::initializeGL()
{
    // Set clear color as black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Select pixel storage mode used by glTexImage2D
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // Create the texture
    glGenTextures(1, &_texture);

    // Retrieve FPS from the camera
    _fps = 30;

    /* Start the timer */
    _tick();
}

void UIOpenGLFrameRenderer::paintGL()
{
    // Clear the screen and depth buffer (with black)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Select the model view matrix and reset it
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, 0.0f);

    // Pull Stereo Frame data from the Output Queue
    FrameData *frame_data;

    if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
        return;
    }

    glEnable(GL_TEXTURE_RECTANGLE_ARB);

    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _texture);

    // Transfer image data to the GPU

    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
                 GL_RGBA, 640, 480, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, frame_data->left_data);
    if (glGetError() != GL_NO_ERROR)
    {
        exit(0);
    }

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);                         glVertex2f(1.0, 1.0);
        glTexCoord2f(640, 0);             glVertex2f(-1.0, 1.0);
        glTexCoord2f(640, 480); glVertex2f(-1.0, -1.0);
        glTexCoord2f(0, 480);             glVertex2f(1.0, -1.0);
    glEnd();

    glDisable(GL_TEXTURE_RECTANGLE_ARB);

    // Free fast memory
    fast_mem_pool_release_memory(frame_data);
}

void UIOpenGLFrameRenderer::resizeGL( int w, int h)
{
    _width = w;
    _height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);  // Select the projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (h == 0)  // Calculate aspect ratio of the window
       gluPerspective(60, (float) w, 1.0, 50.0);
    else
       gluPerspective(60, (float) w / (float) h, 1.0, 50.0);

    gluLookAt(0.0,  0.0, 2.0,   // eye
              0.0,  0.0, 0.0,   // center
              0.0,  1.0, 0.0);  // up

    glMatrixMode(GL_MODELVIEW);  // Select the model view matrix
    glLoadIdentity();           // Reset the model view matrix
}
