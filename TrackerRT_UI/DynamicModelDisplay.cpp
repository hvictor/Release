#include "DynamicModelDisplay.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <iostream>
#include "../OpticalLayer/HSVManager.h"
#include "../Calibration/TennisFieldCalibrator.h"
#include "../Calibration/PerimetralConesDetector.h"
#include "../AugmentedReality/OverlayRenderer.h"
#include "../StaticModel/TennisFieldStaticModel.h"

#define QT_NO_DEBUG_OUTPUT

extern FrameData **pRenderFrameData;
extern SpinlockQueue *outputFramesQueueExternPtr;

DynamicModelDisplay::DynamicModelDisplay(QWidget *parent)
    : QGLWidget(parent),
      clearColor(Qt::black)
{

}

GLWidget::~GLWidget()
{
    makeCurrent();

    doneCurrent();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{

}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void GLWidget::activateFieldCalibration()
{

}

void GLWidget::activatePerimeterCalibration()
{

}

void GLWidget::activateTargetCalibration()
{

}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(640, 480);
}

QSize GLWidget::sizeHint() const
{
    return QSize(640, 480);
}

void GLWidget::renderStereoRawData(uchar *u8data)
{
    update();
}

void GLWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    makeObject();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glPointSize(6.0);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

}

void GLWidget::paintGL()
{
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glOrtho(-10.0, 10.0, -10.0, 10.0, 0.0, 10.0);

    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 5.0);
    glEnd();

}
void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, 640, 480);
}

