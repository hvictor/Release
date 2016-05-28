#ifndef DYNAMICMODELDISPLAY_H
#define DYNAMICMODELDISPLAY_H

#include <QtOpenGL/QGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QRubberBand>
#include <QMouseEvent>

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class DynamicModelDisplay : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit DynamicModelDisplay(QWidget *parent = 0);
    ~DynamicModelDisplay();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void renderStereoRawData(uchar *u8data);
    void setClearColor(const QColor &color);
    void activateTargetCalibration();
    void activateFieldCalibration();
    void activatePerimeterCalibration();

signals:
    void clicked();
    void requestFrame();
    void transmitTargetHSVRange(HSVRange targetHSVRange);
    void transmitFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange);
    void disableCalibControlFLD();
    void enableCalibControlFLD();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:

};

#endif
