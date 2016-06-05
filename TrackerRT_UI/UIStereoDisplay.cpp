#include "UIStereoDisplay.h"
#include "ui_UIStereoDisplay.h"

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"

extern StereoSensorAbstractionLayer *stereoSALExternPtr;
extern SpinlockQueue *outputFramesQueueExternPtr;
FrameData **pRenderFrameData;

// Direct Fetch RAW Stereo Data from the ZED Camera Sensor
extern FrameData directFetchRawStereoData(StereoSensorAbstractionLayer *stereoSAL);

UIStereoDisplay::UIStereoDisplay(QDialog *parent):
    _stereo(true),
    _autoFetch(true)
{
    pRenderFrameData = (FrameData **)malloc(sizeof(FrameData *));

    mainLayout = new QGridLayout;

    QColor clearColor;
    clearColor.setRed(0);
    clearColor.setGreen(0);
    clearColor.setBlue(0);
    clearColor.setAlpha(255);
    /*
    clearColor.setHsv(((0 * 0) + 0) * 255
                      / (0 * 0 - 1),
                      255, 63);
    */

    glWidget = new GLWidget('L');
    glWidgetR = new GLWidget('R');

    glWidget->setClearColor(clearColor);
    glWidgetR->setClearColor(clearColor);

    setWindowTitle(tr("[DISPLAY STEREO]"));

    QObject::connect(glWidget, SIGNAL(floorLinearModelReady()), this, SIGNAL(floorLinearModelReady());
}

void UIStereoDisplay::init(bool stereo, bool autoFetch)
{
    _stereo = stereo;
    _autoFetch = autoFetch;

    mainLayout->addWidget(glWidget, 0, 0);

    if (_stereo) {
        mainLayout->addWidget(glWidgetR, 0, 1);
    }

    setLayout(mainLayout);

    if (_autoFetch) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &UIStereoDisplay::renderStereoRawData);
        timer->start(1);
    }
}

void UIStereoDisplay::calibrateTarget()
{
    glWidget->activateTargetCalibration();
}

void UIStereoDisplay::calibratePerimeter()
{
    glWidget->activatePerimeterCalibration();
}

void UIStereoDisplay::calibrateField()
{
    glWidget->activateFieldCalibration();
}

void UIStereoDisplay::fetch()
{
    FrameData frame_data = directFetchRawStereoData(stereoSALExternPtr);

    glWidget->renderStereoRawData((uchar *)(frame_data.left_data));

    if (_stereo) {
        glWidgetR->renderStereoRawData((uchar *)(frame_data.right_data));
    }
}

void UIStereoDisplay::updateRawData()
{
    FrameData frame_data = directFetchRawStereoData(stereoSALExternPtr);

    glWidget->renderStereoRawData((uchar *)(frame_data.left_data));

    if (_stereo) {
        glWidgetR->setRawData((uchar *)(frame_data.right_data));
    }
}

void UIStereoDisplay::renderStereoRawData()
{
    if (array_spinlock_queue_pull(outputFramesQueueExternPtr, (void **)pRenderFrameData) < 0) {
        return;
    }

    glWidget->renderStereoRawData((uchar *)((*pRenderFrameData)->left_data));

    if (_stereo) {
        glWidgetR->renderStereoRawData((uchar *)((*pRenderFrameData)->right_data));
    }

    fast_mem_pool_release_memory(*pRenderFrameData);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
