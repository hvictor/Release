#include "GLWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <iostream>
#include "../OpticalLayer/HSVManager.h"
#define QT_NO_DEBUG_OUTPUT

extern FrameData **pRenderFrameData;
extern SpinlockQueue *outputFramesQueueExternPtr;

GLWidget::GLWidget(char side, QWidget *parent)
    : QGLWidget(parent),
      clearColor(Qt::black),
      program(0),
      _side(side)
{
    setup = 0;
    u8data = 0;
    rubberBand = 0;
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete texture;
    delete program;
    doneCurrent();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    origin = event->pos();

    if (!rubberBand) {
        QPalette palette;
        palette.setBrush(QPalette::Foreground, QBrush(Qt::yellow));
        palette.setBrush(QPalette::Base, QBrush(Qt::yellow));
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setPalette(palette);
    }

    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{

    QPoint dst = event->pos();
    rubberBand->hide();
    // determine selection, for example using QRect::intersects()
    // and QRect::contains().

    int left = std::min(origin.x(), dst.x());
    int top = std::min(origin.y(), dst.y());
    int width = abs(origin.x() - dst.x());
    int height = abs(origin.y() - dst.y());

    HSVRange hsv_range = HSVManager::getInstance()->getHSVRange((const uint8_t *)u8data, 640, 480, left, top, width, height);

    printf("Using range: %d %d %d - %d %d %d\n", hsv_range.Hmin, hsv_range.Smin, hsv_range.Vmin,
           hsv_range.Hmax, hsv_range.Smax, hsv_range.Vmax);

    HSVManager::getInstance()->filterHSVRange(u8data, 640, 480, hsv_range, u8data);
    printf("UDATA!!! ASSIGNED\n");

    update();

    /*
     *     printf("GLWidget :: mouseReleaseEvent :: Updated\n");
    QRect selectionArea(left, top, width, height);

    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::red));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));
    QRubberBand *r = new QRubberBand(QRubberBand::Rectangle, this);
    r->setPalette(palette);
    r->setGeometry(selectionArea);
    r->show();
    */
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
    this->u8data = u8data;
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

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("texture", 0);
}

void GLWidget::paintGL()
{
    if (u8data == 0)
        return;

    makeObject();

    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;
    m.ortho(-1.0, 1.0, 1.0, -1.0, 0.0, 10.0);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    texture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, 640, 480);
}

void GLWidget::makeObject()
{
    if (u8data == 0) return;

    //FrameData *frame_data;
    static const int coords[4][3] =
    {
        { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
    };

    if (setup) {
        QImage glImage(const_cast<uchar *>(this->u8data), 640, 480, QImage::Format_RGBA8888);

        delete texture;
        texture = new QOpenGLTexture(glImage.mirrored());

// ---------------------------------------------------
//        texture->setData(glImage);
// ---------------------------------------------------

        return;
    }

    if (!setup) { setup = 1; }

    for (int j = 0; j < 1; ++j) {
        QImage glImage(const_cast<uchar *>(this->u8data), 640, 480, QImage::Format_RGBA8888);

        texture = new QOpenGLTexture(glImage.mirrored());
    }


    QVector<GLfloat> vertData;
    for (int j = 0; j < 4; ++j) {
        // Vertex position
        vertData.append(1.0 * coords[j][0]);
        vertData.append(1.0 * coords[j][1]);
        vertData.append(1.0 * coords[j][2]);

        // Texture coordinate
        vertData.append(j == 0 || j == 3);
        vertData.append(j == 0 || j == 1);
    }

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}
