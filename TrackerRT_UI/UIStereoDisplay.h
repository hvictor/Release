#ifndef UISTEREODISPLAY_H
#define UISTEREODISPLAY_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "GLWidget.h"

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../SensorAbstractionLayer/StereoSensorAbstractionLayer.h"

namespace Ui {
class UIStereoDisplay;
}

class UIStereoDisplay : public QWidget
{
    Q_OBJECT

public:
    UIStereoDisplay();
    ~UIStereoDisplay();

private:
    Ui::UIStereoDisplay *ui;
    GLWidget *glWidgetL;
    //GLWidget *glWidgetR;


private slots:
    void renderStereoRawData();
};

#endif // UISTEREODISPLAY_H
