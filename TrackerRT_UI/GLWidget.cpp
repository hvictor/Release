#include "GLWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QOpenGLPixelTransferOptions>
#include <iostream>

bool jesus;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      clearColor(Qt::black),
      program(0)
{
    memset(texture, 0, sizeof(QOpenGLTexture));
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete texture;
    delete program;
    doneCurrent();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(640, 480);
}

QSize GLWidget::sizeHint() const
{
    return QSize(640, 480);
}

void GLWidget::renderStereoRawData()
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
    FrameData *frame_data;

    static int setup = 0;
    static const int coords[4][3] =
    {
        { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
    };

    if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
        return;
    }

    if (setup) {
        /*
        QImage a(QString("/tmp/textures/a.png"));
        a = a.convertToFormat(QImage::Format_RGBA8888);
        if (jesus == true) {
            a = a.mirrored();
            jesus = false;
        }
        else { jesus = true; }
        */

        //QImage glImage((const uchar *)a.constBits(), 640, 480, QImage::Format_RGBA8888);
        QImage glImage((const uchar *)frame_data->left_data, 640, 480, QImage::Format_RGBA8888);
        texture->setData(glImage);
        return;
    }

    if (!setup) { setup = 1; }

    for (int j = 0; j < 1; ++j) {
        /*
        QImage a(QString("/tmp/textures/a.png"));
        a = a.convertToFormat(QImage::Format_RGBA8888);
        if (jesus == true) {
            a = a.mirrored();
            jesus = false;
        }
        else { jesus = true; }
        */
        QImage glImage((const uchar *)frame_data->left_data, 640, 480, QImage::Format_RGBA8888);

        texture = new QOpenGLTexture(glImage);
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
