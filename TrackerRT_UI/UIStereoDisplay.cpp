#include "UIStereoDisplay.h"
#include "ui_UIStereoDisplay.h"

extern SpinlockQueue *outputFramesQueueExternPtr;

//
// Direct Fetch RAW Stereo Data from the ZED Camera Sensor
//
extern FrameData directFetchRawStereoData(StereoSensorAbstractionLayer *stereoSAL);
extern StereoSensorAbstractionLayer *sSAL;

FrameData **pRenderFrameData;

UIStereoDisplay::UIStereoDisplay()
{
    QGridLayout *mainLayout = new QGridLayout;

    pRenderFrameData = (FrameData **)malloc(sizeof(FrameData *));

    QColor clearColor;
    clearColor.setHsv(((0 * 0) + 0) * 255
                      / (0 * 0 - 1),
                      255, 63);

    glWidgetL = new GLWidget;
    glWidgetL->setClearColor(clearColor);
    glWidgetL->renderStereoRawData();

    glWidgetR = new GLWidget;
    glWidgetR->setClearColor(clearColor);
    glWidgetR->renderStereoRawData();

    mainLayout->addWidget(glWidgetL, 0, 0);
    mainLayout->addWidget(glWidgetR, 1, 0);
    setLayout(mainLayout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &UIStereoDisplay::renderStereoRawData);
    timer->start(1);

    setWindowTitle(tr("[STEREO]"));
}

void UIStereoDisplay::renderStereoRawData()
{
    if (array_spinlock_queue_pull(outputFramesQueueExternPtr, (void **)pRenderFrameData) < 0) {
        return;
    }

    glWidgetL->renderStereoRawData();
    glWidgetR->renderStereoRawData();

    fast_mem_pool_release_memory(*pRenderFrameData);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
