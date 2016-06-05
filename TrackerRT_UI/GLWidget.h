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
#include "../Calibration/TennisFieldCalibrator.h"
#include "../OpticalLayer/HSVManager.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(char side, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void renderStereoRawData(uchar *u8data);
    void setRawData(uchar *u8data);
    void setClearColor(const QColor &color);
    void activateTargetCalibration();
    void activateFieldCalibration();
    void activatePerimeterCalibration();

    double GPUMinSegmentLength;
    double GPUMaxSegmentDistance;

signals:
    void clicked();
    void requestFrame();
    void transmitTargetHSVRange(HSVRange targetHSVRange);
    void transmitFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange);
    void disableCalibControlFLD();
    void enableCalibControlFLD();
    void floorLinearModelReady();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void runProbabilisticFieldLinesDetection_GPU();
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
    bool calib_perim;

    TennisFieldCalibrator *calibrator;
};

#endif
