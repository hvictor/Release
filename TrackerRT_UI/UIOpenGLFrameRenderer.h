#ifndef UIOPENGLFRAMERENDERER_H
#define UIOPENGLFRAMERENDERER_H

#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QImage>
#include <QTimer>

namespace Ui {
class UIOpenGLFrameRenderer;
}


class UIOpenGLFrameRenderer : public QGLWidget
{
    Q_OBJECT
public:
    explicit UIOpenGLFrameRenderer(QWidget* parent = 0);
    virtual ~UIOpenGLFrameRenderer();

    /* OpenGL initialization, viewport resizing, and painting */

    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height);

private:
    int _width;
    int _height;
    int _fps;
    GLuint  _texture;

protected slots:
    void _tick();
};

#endif // UIOPENGLFRAMERENDERER_H
