#include "UIMonoDisplay.h"
#include "ui_UIMonoDisplay.h"

#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"

// Direct Fetch RAW Stereo Data from the ZED Camera Sensor
extern StereoSensorAbstractionLayer *stereoSALExternPtr;
extern FrameData directFetchRawStereoData(StereoSensorAbstractionLayer *stereoSAL);

UIMonoDisplay::UIMonoDisplay()
{
    QGridLayout *mainLayout = new QGridLayout;

    QColor clearColor;
    clearColor.setHsv(((0 * 0) + 0) * 255
                      / (0 * 0 - 1),
                      255, 63);

    glWidget = new GLWidget('L');

    mainLayout->addWidget(glWidget, 0, 0);
    setLayout(mainLayout);
}

void UIMonoDisplay::render()
{
    FrameData frame_data = directFetchRawStereoData(stereoSALExternPtr);
    glWidget->renderStereoRawData((const uchar *)frame_data.left_data);
}

UIMonoDisplay::~UIMonoDisplay()
{
    delete ui;
}
