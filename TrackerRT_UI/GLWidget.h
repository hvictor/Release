#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QRubberBand>
#include <QMouseEvent>

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../SensorAbstractionLayer/StereoSensorAbstractionLayer.h"
#include "../OpticalLayer/HSVManager.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QObject, public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(char side, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void renderStereoRawData(uchar *u8data);
    void setClearColor(const QColor &color);
    void activateTargetCalibration();
    void activateFieldCalibration();

signals:
    void clicked();
    void transmitTargetHSVRange(HSVRange targetHSVRange);
    void transmitFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void makeObject();
    char _side;
    int idx;
    int setup;
    uchar *u8data;

    QPoint origin;
    QColor clearColor;
    QPoint lastPos;

    QOpenGLTexture *texture;
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;
    QRubberBand *rubberBand;

    bool calib_tgt;
    bool calib_field;
};

#endif
