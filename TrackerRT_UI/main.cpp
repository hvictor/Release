#include "mainwindow.h"
#include <QApplication>
#include <QSlider>
#include <circularindicator.h>
#include <pthread.h>
#include "UIStereoDisplay.h"

#include <QtGui/QGuiApplication>
#include "UIStereoDisplay.h"

#include <QtCore/qmath.h>

extern void run();
/*
void *run_proc(void *args)
{
    run();
    return NULL;
}
*/

pthread_t runHdl;

int main(int argc, char *argv[])
{
    //pthread_create(&runHdl, 0, run_proc, 0);

    QApplication app(argc, argv);

    UIStereoDisplay stereoDisplay;
    stereoDisplay.show();

    return app.exec();
}
