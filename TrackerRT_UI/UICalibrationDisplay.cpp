#include "UICalibrationDisplay.h"
#include "ui_UICalibrationDisplay.h"

UICalibrationDisplay::UICalibrationDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UICalibrationDisplay)
{
    ui->setupUi(this);
}

UICalibrationDisplay::~UICalibrationDisplay()
{
    delete ui;
}
