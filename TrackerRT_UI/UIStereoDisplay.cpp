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
    glWidget->setClearColor(clearColor);

    glWidgetR = new GLWidget('R');
    //glWidgetR->setClearColor(clearColor);
    //glWidgetR->renderStereoRawData();

    printf("Widgets created\n");

    mainLayout->addWidget(glWidget, 0, 0);
    mainLayout->addWidget(glWidgetR, 0, 1);
    setLayout(mainLayout);

    printf("layout ok\n");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &UIStereoDisplay::renderStereoRawData);
    timer->start(1);

    setWindowTitle(tr("[STEREO]"));
}

void UIStereoDisplay::renderStereoRawData()
{
    printf("Pulling (external)\n");
    if (array_spinlock_queue_pull(outputFramesQueueExternPtr, (void **)pRenderFrameData) < 0) {
        return;
    }

    glWidget->renderStereoRawData((const uchar *)((*pRenderFrameData)->left_data));
    glWidgetR->renderStereoRawData((const uchar *)((*pRenderFrameData)->left_data));

    printf("Releasing (external)");
    fast_mem_pool_release_memory(*pRenderFrameData);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
