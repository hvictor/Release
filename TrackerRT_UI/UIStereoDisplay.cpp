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

    QGridLayout *mainLayout = new QGridLayout;

    QColor clearColor;
    clearColor.setHsv(((0 * 0) + 0) * 255
                      / (0 * 0 - 1),
                      255, 63);

    glWidget = new GLWidget('L');
    //glWidget->setClearColor(clearColor);
    glWidgetR = new GLWidget('R');

    setWindowTitle(tr("[STEREO]"));
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

void UIStereoDisplay::fetch()
{
    FrameData frame_data = directFetchRawStereoData(stereoSALExternPtr);

    glWidget->renderStereoRawData((const uchar *)(frame_data.left_data));

    if (_stereo) {
        glWidgetR->renderStereoRawData((const uchar *)(frame_data.right_data));
    }
}

void UIStereoDisplay::renderStereoRawData()
{
    if (array_spinlock_queue_pull(outputFramesQueueExternPtr, (void **)pRenderFrameData) < 0) {
        return;
    }

    glWidget->renderStereoRawData((const uchar *)((*pRenderFrameData)->left_data));

    if (_stereo) {
        glWidgetR->renderStereoRawData((const uchar *)((*pRenderFrameData)->right_data));
    }

    fast_mem_pool_release_memory(*pRenderFrameData);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
