#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../SensorAbstractionLayer/StereoSensorAbstractionLayer.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(char side, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void renderStereoRawData();
    void setClearColor(const QColor &color);

signals:
    void clicked();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
    void makeObject();
    char _side;
    int idx;

    QColor clearColor;
    QPoint lastPos;

    QOpenGLTexture *texture[2];
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;
};

#endif
