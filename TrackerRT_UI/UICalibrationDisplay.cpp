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
    QObject::connect(ui->btnCalibField, SIGNAL(clicked()), ui->widget, SLOT(calibrateField()));

    QObject::connect(ui->widget->glWidget, SIGNAL(transmitFieldMarkersHSVRange(HSVRange)), this, SLOT(receiveFieldMarkersHSVRange(HSVRange)));
    QObject::connect(ui->widget->glWidget, SIGNAL(transmitTargetHSVRange(HSVRange)), this, SLOT(receiveTargetHSVRange(HSVRange)));
}

UICalibrationDisplay::~UICalibrationDisplay()
{
    delete ui;
}

void UICalibrationDisplay::receiveTargetHSVRange(HSVRange targetHSVRange)
{
    printf("UICalibrationDisplay :: TGT CALIBRATED\n");

    Configuration::getInstance()->calibrationData.targetHSVRange = targetHSVRange;

    printf("Display :: Using filter: %d %d %d - %d %d %d\n", targetHSVRange.Hmin, targetHSVRange.Smin, targetHSVRange.Vmin,
                    targetHSVRange.Hmax, targetHSVRange.Smax, targetHSVRange.Vmax);
}

void UICalibrationDisplay::receiveFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange)
{
    printf("UICalibrationDisplay :: FLD CALIBRATED\n");

    Configuration::getInstance()->calibrationData.fieldMarkersHSVRange = fieldMarkersHSVRange;
}
