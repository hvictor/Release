#include "UIStereoDisplay.h"
#include "ui_UIStereoDisplay.h"

UIStereoDisplay::UIStereoDisplay()
{
    QGridLayout *mainLayout = new QGridLayout;

    QColor clearColor;
    clearColor.setHsv(((0 * 0) + 0) * 255
                      / (0 * 0 - 1),
                      255, 63);

    glWidget = new GLWidget;
    glWidget->setClearColor(clearColor);

    glWidgetR = new GLWidget;
    //glWidgetR->setClearColor(clearColor);
    //glWidgetR->renderStereoRawData();

    mainLayout->addWidget(glWidget, 0, 0);
    //mainLayout->addWidget(glWidgetR, 0, 1);
    setLayout(mainLayout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &UIStereoDisplay::renderStereoRawData);
    timer->start(1);

    setWindowTitle(tr("[STEREO]"));
}

void UIStereoDisplay::renderStereoRawData()
{
    glWidget->renderStereoRawData();
    //glWidgetR->renderStereoRawData();
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
