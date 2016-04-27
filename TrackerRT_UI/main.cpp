#include "mainwindow.h"
#include <QApplication>
#include <QSlider>
#include <circularindicator.h>
#include <pthread.h>
#include "UIStereoDisplay.h"
#include <QtGui/QGuiApplication>
#include "UIStereoDisplay.h"
#include <QtCore/qmath.h>

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../SensorAbstractionLayer/StereoSensorAbstractionLayer.h"

extern StereoSensorAbstractionLayer *stereoSALExternPtr;

// Direct Init
extern void directInit(StereoSensorAbstractionLayer **stereoSAL);

int main(int argc, char *argv[])
{
    /*
    pthread_create(&runHdl, 0, run_proc, 0);

    while (!systemReady) {
        usleep(10);
    }
    */

    directInit(&stereoSALExternPtr);
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    //UIStereoDisplay stereoDisplay;
    //stereoDisplay.show();

    return app.exec();
}
