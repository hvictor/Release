#include "UIStereoDisplay.h"
#include "ui_UIStereoDisplay.h"

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"

extern SpinlockQueue *outputFramesQueueExternPtr;
FrameData **pRenderFrameData;

UIStereoDisplay::UIStereoDisplay()
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

    mainLayout->addWidget(glWidget, 0, 0);
    mainLayout->addWidget(glWidgetR, 0, 1);
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

    glWidget->renderStereoRawData((const uchar *)((*pRenderFrameData)->left_data));
    glWidgetR->renderStereoRawData((const uchar *)((*pRenderFrameData)->right_data));

    if ((*pRenderFrameData)->depth_data != 0) {
        printf("[Depth] Measure present\n");
        //printf("[Depth]  Frame center measures an object at: %d [m]", (*pRenderFrameData)->depth_data[640 * (480/2) + 640/2]);
    }


    fast_mem_pool_release_memory(*pRenderFrameData);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
