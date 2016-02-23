#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "circularindicator.h"
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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


}

MainWindow::~MainWindow()
{
    delete ui;
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
