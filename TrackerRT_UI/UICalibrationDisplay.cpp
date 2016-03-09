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

    ui->mdiArea->setBackground(QImage("/home/sled/master/final/TrackerRT_UI/TrackerRT_UI/img.jpg"));
}

UICalibrationDisplay::~UICalibrationDisplay()
{
    delete ui;
}
