#include "duoconfigwidget.h"
#include "ui_duoconfigwidget.h"

DUOConfigWidget::DUOConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DUOConfigWidget)
{
    uiModel = UIModel::getInstance();

    ui->setupUi(this);

    ui->lcdGain->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdGain->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdExposure->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdExposure->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdIRLED->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdIRLED->setStyleSheet("background-color: black; color: lightgreen;");

    QObject::connect(ui->duoGainSlider, SIGNAL(valueChanged(int)), uiModel, SLOT(setDUOGain(int)));
    QObject::connect(ui->duoExposureSlider, SIGNAL(valueChanged(int)), uiModel, SLOT(setDUOExposure(int)));
    QObject::connect(ui->duoIRLEDSlider, SIGNAL(valueChanged(int)), uiModel, SLOT(setDUOIRLedArrayIntensity(int)));
}

DUOConfigWidget::~DUOConfigWidget()
{
    delete ui;
}
