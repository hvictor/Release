#include "UICalibrationDisplay.h"
#include "ui_UICalibrationDisplay.h"

UICalibrationDisplay::UICalibrationDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UICalibrationDisplay),
    pixmap("img.jpg")
{
    ui->setupUi(this);

    ui->lcdGPUDistance->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGPUDistance->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdGPUSegments->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGPUSegments->setStyleSheet("background-color: black; color: lightgreen;");

    QObject::connect(ui->btnAcquireFrame, SIGNAL(clicked()), ui->monoDisplay, SLOT(render()));
}

UICalibrationDisplay::~UICalibrationDisplay()
{
    delete ui;
}
