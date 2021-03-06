#include "UICalibrationDisplay.h"
#include "ui_UICalibrationDisplay.h"
#include "../Configuration/Configuration.h"

UICalibrationDisplay::UICalibrationDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UICalibrationDisplay)
{
    ui->setupUi(this);

    ui->lcdGPUDistance->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGPUDistance->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdGPUSegments->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGPUSegments->setStyleSheet("background-color: black; color: lightgreen;");

    QObject::connect(ui->btnAcquireFrame, SIGNAL(clicked()), ui->widget, SLOT(fetch()));
    ui->widget->init(false, false);

    QObject::connect(ui->btnCalibTgt, SIGNAL(clicked()), ui->widget, SLOT(calibrateTarget()));
    QObject::connect(ui->btnCalibPrm, SIGNAL(clicked()), ui->widget, SLOT(calibratePerimeter()));
    QObject::connect(ui->btnCalibFld, SIGNAL(clicked()), ui->widget, SLOT(calibrateField()));
    QObject::connect(ui->btnCalibNet, SIGNAL(clicked()), ui->widget, SLOT(calibrateNet()));


    QObject::connect(ui->widget->glWidget, SIGNAL(transmitFieldMarkersHSVRange(HSVRange)), this, SLOT(receiveFieldMarkersHSVRange(HSVRange)));
    QObject::connect(ui->widget->glWidget, SIGNAL(transmitTargetHSVRange(HSVRange)), this, SLOT(receiveTargetHSVRange(HSVRange)));
    QObject::connect(ui->widget->glWidget, SIGNAL(requestFrame()), this, SLOT(provideFrame()));

    QObject::connect(ui->gpuMinSegmentLengthSlider, SIGNAL(valueChanged(int)), this, SLOT(setGPUMinSegmentLength(int)));
    QObject::connect(ui->gpuMaxSegmentDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(setGPUMaxSegmentDistance(int)));

    QObject::connect(ui->widget->glWidget, SIGNAL(disableCalibControlFLD()), this, SLOT(disableCalibBtnFLD()));
    QObject::connect(ui->widget->glWidget, SIGNAL(enableCalibControlFLD()), this, SLOT(enableCalibBtnFLD()));

    QObject::connect(ui->widget->glWidget, SIGNAL(floorLinearModelReady()), this, SIGNAL(floorLinearModelReady()));

    ui->btnCalibFld->setDisabled(true);

    ui->widget->glWidget->GPUMinSegmentLength = 20;
    ui->widget->glWidget->GPUMaxSegmentDistance = 100;
}

UICalibrationDisplay::~UICalibrationDisplay()
{
    delete ui;
}

void UICalibrationDisplay::enableCalibBtnFLD()
{
    ui->btnCalibFld->setEnabled(true);
}

void UICalibrationDisplay::disableCalibBtnFLD()
{
    ui->btnCalibFld->setDisabled(true);
}

void UICalibrationDisplay::setGPUMinSegmentLength(int minSegmentLen)
{
    ui->widget->glWidget->GPUMinSegmentLength = minSegmentLen;
}

void UICalibrationDisplay::setGPUMaxSegmentDistance(int maxSegmentDistance)
{
    ui->widget->glWidget->GPUMaxSegmentDistance = maxSegmentDistance;
}

void UICalibrationDisplay::provideFrame()
{
    ui->widget->updateRawData();
}

void UICalibrationDisplay::receiveTargetHSVRange(HSVRange targetHSVRange)
{
    printf("UICalibrationDisplay :: TGT CALIBRATED\n");

    Configuration::getInstance()->calibrationData.targetHSVRange = targetHSVRange;
}

void UICalibrationDisplay::receiveFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange)
{
    printf("UICalibrationDisplay :: FLD CALIBRATED\n");

    Configuration::getInstance()->calibrationData.fieldMarkersHSVRange = fieldMarkersHSVRange;
}
