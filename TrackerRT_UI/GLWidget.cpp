#include "GLWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <iostream>
#define QT_NO_DEBUG_OUTPUT
bool jesus;
extern SpinlockQueue *outputFramesQueueExternPtr;

//
// Direct Fetch RAW Stereo Data from the ZED Camera Sensor
//
extern FrameData directFetchRawStereoData(StereoSensorAbstractionLayer *stereoSAL);
extern StereoSensorAbstractionLayer *sSAL;

GLWidget::GLWidget(char side, QWidget *parent)
    : QGLWidget(parent),
      clearColor(Qt::black),
      program(0),
      _side(side)
{
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
    printf("[%c] paintGL: Calling makeObject\n", _side);
    makeObject();
    printf("[%c] paintGL: OK makeObject, Clearing color...\n", _side);

    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    printf("[%c] paintGL: OK clearing color\n", _side);

    QMatrix4x4 m;
    m.ortho(-1.0, 1.0, 1.0, -1.0, 0.0, 10.0);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    printf("[%c] paintGL: binding, texture = %p\n", _side, texture);
    texture->bind();
    printf("[%c] NOT Drawing triangle fan\n", _side);
    if (_side == 'R') glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    printf("[%c] OK Drawing triangle fan\n", _side);
    printf("[%c] paintGL: OK binding\n", _side);
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

    // Fetch STEREO Frame Data from Output Queue
/////////////
/*
    static int measure_counter = 0;
    static double queue_count_mean = 0;

    queue_count_mean += (double)outputFramesQueueExternPtr->count;

    if (++measure_counter == 100) {
        queue_count_mean /= measure_counter;
        measure_counter = 0;
        printf("[queue avg] current size: %.4f frames\n", queue_count_mean);
        queue_count_mean = 0;
    }
*/
///////////

    if (array_spinlock_queue_pull(outputFramesQueueExternPtr, (void **)&frame_data) < 0) {
        return;
    }

    //DIRECT:
    //FrameData fData = directFetchRawStereoData(sSAL);

    if (setup) {
        QImage glImage((const uchar *)frame_data->left_data, 640, 480, QImage::Format_RGBA8888);

        fast_mem_pool_release_memory(frame_data);

        delete texture;
        texture = new QOpenGLTexture(glImage);

// ---------------------------------------------------
//        texture->setData(glImage);
// ---------------------------------------------------
        return;
    }

    if (!setup) { setup = 1; }

    for (int j = 0; j < 1; ++j) {
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

    fast_mem_pool_release_memory(frame_data);
}
