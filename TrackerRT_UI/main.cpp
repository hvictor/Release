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

extern void run();
extern volatile bool systemReady;
volatile bool systemCalibrated;
volatile bool systemRecording;
extern StereoSensorAbstractionLayer *stereoSALExternPtr;

pthread_t runHdl;

void *run_proc(void *args)
{
    run();

    return NULL;
}

// Direct Init
extern void directInit(StereoSensorAbstractionLayer **stereoSAL);

int main(int argc, char *argv[])
{
    systemCalibrated = false;

    pthread_create(&runHdl, 0, run_proc, 0);

    while (!systemReady) {
        usleep(10);
    }

    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    //UIStereoDisplay stereoDisplay;
    //stereoDisplay.show();

    return app.exec();
}
