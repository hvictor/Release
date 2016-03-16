#include "mainwindow.h"
#include <QApplication>
#include <QSlider>
#include <circularindicator.h>
#include <UIOpenGLFrameRenderer.h>
#include <pthread.h>

extern void run();

void *run_proc(void *args)
{
    run();
    return NULL;
}

pthread_t runHdl;

int main(int argc, char *argv[])
{
    pthread_create(&runHdl, 0, run_proc, 0);

    QApplication a(argc, argv);

    UIOpenGLFrameRenderer uiGLFrameRenderer;
    uiGLFrameRenderer.show();

    return a.exec();

    //MainWindow w;
    //w.show();

    //return a.exec();
}
