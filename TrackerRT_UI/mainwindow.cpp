#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "circularindicator.h"
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>
#include "UICalibrationDisplay.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    uiModel = UIModel::getInstance();

    ui->setupUi(this);

    ui->recordToBtn->setVisible(false);

    ui->lcdPeakCPU0->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPeakCPU0->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdPeakCPU1->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPeakCPU1->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdPeakCPU2->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPeakCPU2->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdPeakCPU3->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPeakCPU3->setStyleSheet("background-color: black; color: lightgreen;");

    ui->lcdPeakGPU->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPeakGPU->setStyleSheet("background-color: black; color: red;");

    ui->mdiArea->setWindowFlags(Qt::FramelessWindowHint);

    CircularIndicator *c1 = new CircularIndicator("CPU 0", QRectF(7.0, 7.0, 80.0, 80.0));
    CircularIndicator *c2 = new CircularIndicator("CPU 1", QRectF(7.0, 7.0, 80.0, 80.0));
    CircularIndicator *c3 = new CircularIndicator("CPU 2", QRectF(7.0, 7.0, 80.0, 80.0));
    CircularIndicator *c4 = new CircularIndicator("CPU 3", QRectF(7.0, 7.0, 80.0, 80.0));
    CircularIndicator *cGPU = new CircularIndicator("GPU\nNVIDIA Tegra K1", QRectF(7.0, 7.0, 135.0, 135.0));
    cGPU->setGPU();

    QObject::connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),c1,SLOT(setProgress(int)));
    QObject::connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(cpuCoreChanged(int)));

    QObject::connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)),c2,SLOT(setProgress(int)));
    QObject::connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(cpuCoreChanged(int)));

    QObject::connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)),c3,SLOT(setProgress(int)));
    QObject::connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(cpuCoreChanged(int)));

    QObject::connect(ui->horizontalSlider_5, SIGNAL(valueChanged(int)),c4,SLOT(setProgress(int)));
    QObject::connect(ui->horizontalSlider_5, SIGNAL(valueChanged(int)), this, SLOT(cpuCoreChanged(int)));

    QObject::connect(ui->horizontalSlider_6, SIGNAL(valueChanged(int)),cGPU,SLOT(setProgress(int)));
    QObject::connect(ui->horizontalSlider_6, SIGNAL(valueChanged(int)), this, SLOT(cpuCoreChanged(int)));

    QMdiSubWindow *subWindow1 = new QMdiSubWindow(ui->mdiArea);
    subWindow1->setWidget(c1);
    subWindow1->setFixedSize(95, 95);
    subWindow1->setWindowFlags(Qt::FramelessWindowHint);
    subWindow1->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(subWindow1);

    QMdiSubWindow *subWindow2 = new QMdiSubWindow(ui->mdiArea_2);
    subWindow2->setWidget(c2);
    subWindow2->setFixedSize(95, 95);
    subWindow2->setWindowFlags(Qt::FramelessWindowHint);
    subWindow2->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea_2->addSubWindow(subWindow2);

    QMdiSubWindow *subWindow3 = new QMdiSubWindow(ui->mdiArea_3);
    subWindow3->setWidget(c3);
    subWindow3->setFixedSize(95, 95);
    subWindow3->setWindowFlags(Qt::FramelessWindowHint);
    subWindow3->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea_3->addSubWindow(subWindow3);

    QMdiSubWindow *subWindow4 = new QMdiSubWindow(ui->mdiArea_4);
    subWindow4->setWidget(c4);
    subWindow4->setFixedSize(95, 95);
    subWindow4->setWindowFlags(Qt::FramelessWindowHint);
    subWindow4->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea_4->addSubWindow(subWindow4);

    QMdiSubWindow *subWindowGPU = new QMdiSubWindow(ui->mdiArea_5);
    subWindowGPU->setWidget(cGPU);
    subWindowGPU->setFixedSize(160, 160);
    subWindowGPU->setWindowFlags(Qt::FramelessWindowHint);
    subWindowGPU->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea_5->addSubWindow(subWindowGPU);

    QObject::connect(ui->calibratorBtn, SIGNAL(clicked()), this, SLOT(startCalibrator()));

    // Processing Mode
    QObject::connect(ui->processingModeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setProcessingMode(int)));
    QObject::connect(ui->processingModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateViewProcessingMode(int)));

    // Input Device
    QObject::connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setInputDevice(int)));

    // Frame Info
    QObject::connect(ui->inputFrameTypeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setFrameChannels(int)));
    QObject::connect(ui->outputFrameTypeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setFrameOutputType(int)));
    QObject::connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setInputDevice(int)));
    QObject::connect(ui->frameWidthEdit, SIGNAL(textChanged(QString)), uiModel, SLOT(setFrameWidth(QString)));
    QObject::connect(ui->frameHeightEdit, SIGNAL(textChanged(QString)), uiModel, SLOT(setFrameHeight(QString)));

    QObject::connect(ui->recordToBtn, SIGNAL(clicked(bool)), this, SLOT(chooseRecordDirectory()));
    QObject::connect(ui->startApplicationBtn, SIGNAL(clicked(bool)), this, SLOT(startApplication()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startCalibrator()
{
    UICalibrationDisplay *uiCalibratorDisplay = new UICalibrationDisplay();
    uiCalibratorDisplay->show();
}

void MainWindow::cpuCoreChanged(int value)
{
    QObject *sender = QObject::sender();

    if (sender == ui->horizontalSlider)
        this->ui->lcdPeakCPU0->display(CircularIndicator::cpu_freq[value]);
    else if (sender == ui->horizontalSlider_3)
        this->ui->lcdPeakCPU1->display(CircularIndicator::cpu_freq[value]);
    else if (sender == ui->horizontalSlider_4)
        this->ui->lcdPeakCPU2->display(CircularIndicator::cpu_freq[value]);
    else if (sender == ui->horizontalSlider_5)
        this->ui->lcdPeakCPU3->display(CircularIndicator::cpu_freq[value]);
    else if (sender == ui->horizontalSlider_6)
        this->ui->lcdPeakGPU->display(CircularIndicator::gpu_freq[value]);
        //this->ui->labelPeakGPU->setText(QString("Peak ") + QString::number(CircularIndicator::gpu_freq[value]) + " [MHz]");
}

void MainWindow::updateViewProcessingMode(int processingMode)
{
    if (processingMode == 0) {
        ui->recordToBtn->setVisible(false);
    }

    else {
        ui->recordToBtn->setVisible(true);
    }
}

void MainWindow::chooseRecordDirectory()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly);
    dialog->show();
}

void MainWindow::startApplication()
{
    uiModel->apply();
}