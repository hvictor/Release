#include "duoconfigwidget.h"
#include "ui_duoconfigwidget.h"

DUOConfigWidget::DUOConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DUOConfigWidget)
{
    ui->setupUi(this);

    ui->lcdGain->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGain->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdExposure->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdExposure->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdIRLED->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdIRLED->setStyleSheet("background-color: black; color: lightgreen;");
}

DUOConfigWidget::~DUOConfigWidget()
{
    delete ui;
}
