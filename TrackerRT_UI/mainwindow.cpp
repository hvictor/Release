#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "circularindicator.h"
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>
#include <pthread.h>
#include "UIStereoDisplay.h"
#include "UICalibrationDisplay.h"
#include "../StaticModel/GroundModel.h"

extern volatile bool systemCalibrated;
extern volatile bool systemRecording;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    uiModel = UIModel::getInstance();

    ui->setupUi(this);

    ui->recordToBtn->setVisible(false);
/*
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
    */

    QObject::connect(ui->calibratorBtn, SIGNAL(clicked()), this, SLOT(startCalibrator()));
    QObject::connect(ui->btnSaveConfig, SIGNAL(clicked()), this, SLOT(saveConfig()));
    QObject::connect(ui->btnLoadConfig, SIGNAL(clicked()), this, SLOT(loadConfiguration()));

    // Processing Mode
    QObject::connect(ui->processingModeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setProcessingMode(int)));
    QObject::connect(ui->processingModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateViewProcessingMode(int)));

    // Input Device
    QObject::connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setInputDevice(int)));

    // Frame Info
    //QObject::connect(ui->inputFrameTypeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setFrameChannels(int)));
    //QObject::connect(ui->outputFrameTypeComboBox, SIGNAL(currentIndexChanged(int)), uiModel, SLOT(setFrameOutputType(int)));
    //QObject::connect(ui->frameWidthEdit, SIGNAL(textChanged(QString)), uiModel, SLOT(setFrameWidth(QString)));
    //QObject::connect(ui->frameHeightEdit, SIGNAL(textChanged(QString)), uiModel, SLOT(setFrameHeight(QString)));

    QObject::connect(ui->recordToBtn, SIGNAL(clicked(bool)), this, SLOT(chooseRecordDirectory()));
    QObject::connect(ui->startApplicationBtn, SIGNAL(clicked(bool)), this, SLOT(startApplication()));
    QObject::connect(ui->stopApplicationBtn, SIGNAL(clicked(bool)), this, SLOT(stopApplication()));

    QObject::connect(ui->fldModel_sensitivityEpsSlider, SIGNAL(valueChanged(int)), this, SLOT(updateStaticModel_LinesSensitivityEPS(int)));

    QObject::connect(ui->lowPassXSlider, SIGNAL(valueChanged(int)), this, SLOT(updateOpticalLayerParam_LowPassFilterX(int)));
    QObject::connect(ui->lowPassYSlider, SIGNAL(valueChanged(int)), this, SLOT(updateOpticalLayerParam_LowPassFilterY(int)));

    QObject::connect(ui->slider_DFI, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_DFI(int)));

    QObject::connect(ui->checkBox_enableDepthMeasurements, SIGNAL(toggled(bool)), this, SLOT(toggleDepthMeasurementsStatus(bool)));
    QObject::connect(ui->checkBox_tgtPred_UseWnd, SIGNAL(toggled(bool)), this, SLOT(trackingWindowUseToggled(bool)));
    QObject::connect(ui->checkBox_tgtPred_VisualizeWnd, SIGNAL(toggled(bool)), this, SLOT(trackingWindowVisualizeToggled(bool)));
    QObject::connect(ui->checkBox_enableInputKF, SIGNAL(toggled(bool)), this, SLOT(updateDynamicModel_UseInputKalmanFilter(bool)));

    QObject::connect(ui->radioButton_tgtPred_AdaptiveTWND, SIGNAL(toggled(bool)), this, SLOT(trackingWindowSetAdaptiveTrackingWindow(bool)));
    QObject::connect(ui->radioButton_tgtPred_StaticTWND, SIGNAL(toggled(bool)), this, SLOT(trackingWindowSetStaticTrackingWindow(bool)));
    QObject::connect(ui->slider_tgtPred_TrackingWndSize, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_SetTrackingWindowSize(int)));

    QObject::connect(ui->checkBox_playSettings_FreePlay, SIGNAL(toggled(bool)), this, SLOT(updateDynamicModel_SetFreePlay(bool)));

    QObject::connect(ui->doubleSpinBox_AdaptiveTrackingWndX, SIGNAL(valueChanged(double)), this, SLOT(updateDynamicModel_SetAdaptiveTrackingWndX(double)));
    QObject::connect(ui->doubleSpinBox_AdaptiveTrackingWndY, SIGNAL(valueChanged(double)), this, SLOT(updateDynamicModel_SetAdaptiveTrackingWndY(double)));

    QObject::connect(ui->doubleSpinBox_AcquisitionPeriod, SIGNAL(valueChanged(double)), this, SLOT(updateHardwareParam_SetAcquisitionPeriod(double)));

    QObject::connect(ui->slider_Confidence, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_Confidence(int)));

    QObject::connect(ui->slider_FloorDistTol, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_SetImpactFloorDistTolerance(int)));

    /*
    QObject::connect(ui->spinBoxFloorLinearModelX, SIGNAL(valueChanged(double)), this, SLOT(updateStaticModel_SetFloorPlaneLinearModelFactorX(double)));
    QObject::connect(ui->spinBoxFloorLinearModelY, SIGNAL(valueChanged(double)), this, SLOT(updateStaticModel_SetFloorPlaneLinearModelFactorY(double)));
    QObject::connect(ui->spinBoxFloorLinearModelZ, SIGNAL(valueChanged(double)), this, SLOT(updateStaticModel_SetFloorPlaneLinearModelFactorZ(double)));
    QObject::connect(ui->spinBoxFloorLinearModelD, SIGNAL(valueChanged(double)), this, SLOT(updateStaticModel_SetFloorPlaneLinearModelFactorD(double)));
    */

    QObject::connect(ui->sliderNetHeight, SIGNAL(valueChanged(int)), this, SLOT(updateStaticModel_SetNetHeight(int)));
    QObject::connect(ui->sliderMeasSMP, SIGNAL(valueChanged(int)), this, SLOT(updateStaticModel_SetStaticModelMeasureSamples(int)));

    QObject::connect(ui->radioButton_TwoPlayers, SIGNAL(toggled(bool)), this, SLOT(updatePlayModel_SetTwoPlayersLogic(bool)));
    QObject::connect(ui->radioButton_SinglePlayer, SIGNAL(toggled(bool)), this, SLOT(updatePlayModel_SetSinglePlayerLogic(bool)));

    QObject::connect(ui->checkBox_EnableTGTLST, SIGNAL(toggled(bool)), this, SLOT(toggleNotifyTGTLSTStatus(bool)));
    QObject::connect(ui->slider_TGTLST_Thresh, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_SetTargetLostThreshold(int)));

    QObject::connect(ui->slider_ImpactVectorAngleThresh, SIGNAL(valueChanged(int)), this, SLOT(updateDynamicModel_SetVelocityVectorAngleThresh(int)));

    QObject::connect(ui->slider_PlaybackSpeed, SIGNAL(valueChanged(int)), this, SLOT(updatePlaybackParameters_SetPlaybackLatencyDivisor(int)));

    ui->recordToBtn->setVisible(false);
    // Instantiate Stereo Display
    stereoDisplay = new UIStereoDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadConfiguration()
{
    Configuration::getInstance()->loadConfigFile("/home/ubuntu/Release/config_recording.xml");
    printf("Configuration :: Loaded configuration file\n");
}

void MainWindow::startCalibrator()
{
    UICalibrationDisplay *uiCalibratorDisplay = new UICalibrationDisplay();

    QObject::connect(uiCalibratorDisplay, SIGNAL(floorLinearModelReady(void)), this, SLOT(showFloorLinearModelCoefficients(void)));

    uiCalibratorDisplay->show();
}

void MainWindow::showFloorLinearModelCoefficients()
{
    printf("Static Model :: Floor Linear Model :: Received coefficients ready signal\n");

    PlaneLinearModel floorLinearModel = GroundModel::getInstance()->getGroundPlaneLinearModel();

    /*
    ui->spinBoxFloorLinearModelX->setValue(floorLinearModel.a * 0.001);
    ui->spinBoxFloorLinearModelY->setValue(floorLinearModel.b * 0.001);
    ui->spinBoxFloorLinearModelZ->setValue(floorLinearModel.c * 0.001);
    ui->spinBoxFloorLinearModelD->setValue(floorLinearModel.d * 0.001);
    */
}

void MainWindow::updatePlaybackParameters_SetPlaybackLatencyDivisor(int value)
{
    Configuration::getInstance()->playbackParameters.playbackLatencyDivisor = value;
    printf("Playback Parameters :: Update :: Playback Latency Divisor set to %d\n", Configuration::getInstance()->playbackParameters.playbackLatencyDivisor);
}

void MainWindow::setInputDevice(int dev)
{
    if (dev == 0) {
        Configuration::getInstance()->operationalMode.inputDevice = StereoCameraZED;
        printf("Input Device: StereoLabs ZED Stereo Camera\n");
    }
    else if (dev == 3) {
        Configuration::getInstance()->operationalMode.inputDevice = StereoCameraVirtual;
        printf("Input Device: Virtual Stereo Camera\n");
    }
}

void MainWindow::trackingWindowSetAdaptiveTrackingWindow(bool status)
{
    if (status) {
        updateDynamicModel_SetTrackingWindowMode(AdaptiveTrackingWindow);
    }
}

void MainWindow::updateDynamicModel_SetVelocityVectorAngleThresh(int value)
{
    Configuration::getInstance()->dynamicModelParameters.impact2DVelocityVectorAngleThresh = (double)value;
    printf("Dynamic Model :: Update :: Velocity vector angle thresh = %.2f [Degrees]\n", Configuration::getInstance()->dynamicModelParameters.impact2DVelocityVectorAngleThresh);
}

void MainWindow::updateDynamicModel_SetTargetLostThreshold(int value)
{
    Configuration::getInstance()->dynamicModelParameters.targetPredatorTGTLOSTFramesThreshold = value;
    printf("Dynamic Model :: Update :: TGT LST = %d [Frames]\n", Configuration::getInstance()->dynamicModelParameters.targetPredatorTGTLOSTFramesThreshold);
}

void MainWindow::trackingWindowSetStaticTrackingWindow(bool status)
{
    if (status) {
        updateDynamicModel_SetTrackingWindowSize(ui->slider_tgtPred_TrackingWndSize->value());
        updateDynamicModel_SetTrackingWindowMode(StaticTrackingWindow);
    }
}

void MainWindow::updateStaticModel_SetNetHeight(int value)
{
    Configuration::getInstance()->staticModelParameters.netHeight = (double)value;
    printf("Static Model :: Update :: Net height = %.2f [mm]\n", Configuration::getInstance()->staticModelParameters.netHeight);
}

void MainWindow::updateStaticModel_SetStaticModelMeasureSamples(int value)
{
    Configuration::getInstance()->staticModelParameters.groundPlaneModelDepthSamples = value;
    printf("Static Model :: Update :: Depth Measure Samples = %d\n", Configuration::getInstance()->staticModelParameters.groundPlaneModelDepthSamples);
}

void MainWindow::updateHardwareParam_SetAcquisitionPeriod(double value)
{
    Configuration::getInstance()->zedHardwareParameters.acquisitionPeriod = value;
    printf("Hardware Parameters :: Update :: Acquisition Period set to %.2f [ms]\n", Configuration::getInstance()->zedHardwareParameters.acquisitionPeriod);
}

void MainWindow::updateStaticModel_LinesSensitivityEPS(int value)
{
    Configuration::getInstance()->setStaticModelLinesSensitivityEPS(value);
}

void MainWindow::updateStaticModel_SetFloorPlaneLinearModelFactorX(double a)
{
    GroundModel::getInstance()->setGroundPlaneLinearModelFactorX(a);
    printf("Static Model :: Update :: Floor Linear Plane Factor X: %.2f\n", GroundModel::getInstance()->getGroundPlaneLinearModel().a);
}

void MainWindow::updateStaticModel_SetFloorPlaneLinearModelFactorY(double b)
{
    GroundModel::getInstance()->setGroundPlaneLinearModelFactorY(b);
    printf("Static Model :: Update :: Floor Linear Plane Factor Y: %.2f\n", GroundModel::getInstance()->getGroundPlaneLinearModel().b);
}

void MainWindow::updateStaticModel_SetFloorPlaneLinearModelFactorZ(double c)
{
    GroundModel::getInstance()->setGroundPlaneLinearModelFactorZ(c);
    printf("Static Model :: Update :: Floor Linear Plane Factor Z: %.2f\n", GroundModel::getInstance()->getGroundPlaneLinearModel().c);
}

void MainWindow::updateStaticModel_SetFloorPlaneLinearModelFactorD(double d)
{
    GroundModel::getInstance()->setGroundPlaneLinearModelFactorD(d);
    printf("Static Model :: Update :: Floor Linear Plane Factor D: %.2f\n", GroundModel::getInstance()->getGroundPlaneLinearModel().d);
}

void MainWindow::updateDynamicModel_Confidence(int value)
{
    Configuration::getInstance()->dynamicModelParameters.confidenceThreshold = value;
    printf("Dynamic Model :: Update :: Confidence Threshold = %d\n", Configuration::getInstance()->dynamicModelParameters.confidenceThreshold);
}

void MainWindow::updateDynamicModel_DFI(int value)
{
    ZEDStereoCameraHardwareParameters zedParam = Configuration::getInstance()->getZEDStereoCameraHardwareParameters();
    zedParam.depthFrameInterleave = value;
    Configuration::getInstance()->setZEDStereoCameraHardwareParameters(zedParam);
    Configuration::getInstance()->publish();
}

void MainWindow::updateOpticalLayerParam_LowPassFilterX(int value)
{
    OpticalLayerParameters param = Configuration::getInstance()->getOpticalLayerParameters();

    param.linearLowPassFilterX = ((double)value) / 100.0;

    Configuration::getInstance()->setOpticalLayerParameters(param);
    TargetPredator::getInstance()->update_filter_param();
}

void MainWindow::updateOpticalLayerParam_LowPassFilterY(int value)
{
    OpticalLayerParameters param = Configuration::getInstance()->getOpticalLayerParameters();

    param.linearLowPassFilterY = ((double)value) / 100.0;

    Configuration::getInstance()->setOpticalLayerParameters(param);
    TargetPredator::getInstance()->update_filter_param();
}

void MainWindow::updatePlayModel_SetTwoPlayersLogic(bool status)
{
    if (!status) return;

    Configuration::getInstance()->playLogicParameters.playLogicType = TwoPlayers;
    printf("Play Model :: Update :: Setting Two-Players Logic\n");
}

void MainWindow::updatePlayModel_SetSinglePlayerLogic(bool status)
{
    if (!status) return;

    Configuration::getInstance()->playLogicParameters.playLogicType = SinglePlayer;
    printf("Play Model :: Update :: Setting Single-Player Logic\n");
}

void MainWindow::cpuCoreChanged(int value)
{
    /*
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
        */
}

void MainWindow::toggleDepthMeasurementsStatus(bool status)
{
    if (!status) {
        ui->label_DFI->setEnabled(false);
        ui->slider_DFI->setEnabled(false);
        ui->lcd_DFI->setEnabled(false);
        updateDynamicModel_DFI(0);
    }
    else {
        ui->label_DFI->setEnabled(true);
        ui->slider_DFI->setEnabled(true);
        ui->lcd_DFI->setEnabled(true);
        updateDynamicModel_DFI((int)ui->lcd_DFI->value());
    }
}

void MainWindow::toggleNotifyTGTLSTStatus(bool status)
{
    if (status)
    {
        ui->label_NotifyTGTLST->setEnabled(true);
        ui->slider_TGTLST_Thresh->setEnabled(true);
        ui->lcd_TGTLST->setEnabled(true);
        Configuration::getInstance()->dynamicModelParameters.notifyTGTLostToModel = true;
        printf("Dynamic Model :: Update :: Enabled Notification of TGT LST flag to Dynamic Model\n");
    }
    else
    {
        ui->label_NotifyTGTLST->setEnabled(false);
        ui->slider_TGTLST_Thresh->setEnabled(false);
        ui->lcd_TGTLST->setEnabled(false);
        Configuration::getInstance()->dynamicModelParameters.notifyTGTLostToModel = false;
        printf("Dynamic Model :: Update :: Disabled Notification of TGT LST flag to Dynamic Model\n");
    }
}

void MainWindow::updateViewProcessingMode(int processingMode)
{
    if (processingMode == 0) {
        ui->recordToBtn->setVisible(false);
        Configuration::getInstance()->operationalMode.processingMode = Tracking;
        printf("Operational Mode :: Update :: Tracking Mode is active\n");
        ui->startApplicationBtn->setText("START");
    }

    else if (processingMode == 1) {
        ui->recordToBtn->setVisible(true);
        Configuration::getInstance()->operationalMode.processingMode = Record;
        printf("Operational Mode :: Update :: Recording Mode is active\n");
        ui->startApplicationBtn->setText("REC");
    }
}

void MainWindow::chooseRecordDirectory()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly);
    dialog->show();
}

void MainWindow::saveConfig()
{
    Configuration::getInstance()->writeConfigFile("/home/ubuntu/Release/config_recording.xml");
    printf("Configuration file written.\n");
}

void MainWindow::startApplication()
{
    systemRecording = true;
    systemCalibrated = true;

    if (Configuration::getInstance()->operationalMode.processingMode == Tracking) {
        stereoDisplay->init(false, true);
        stereoDisplay->show();
    }
}

void MainWindow::stopApplication()
{
    systemRecording = false;
}

void MainWindow::updateDynamicModel_UseInputKalmanFilter(bool status)
{
    Configuration::getInstance()->dynamicModelParameters.useInputKalmanFilter = status;

    printf("Dynamic Model :: Update :: Input Kalman Filter Status: %d\n", status);
}

void MainWindow::updateDynamicModel_SetAdaptiveTrackingWndX(double fx)
{
    Configuration::getInstance()->dynamicModelParameters.trackingWndAdaptiveFactorX = fx;

    printf("Dynamic Model :: Update :: Adaptive Tracking Window: Factor X: %.2f\n", fx);
}

void MainWindow::updateDynamicModel_SetAdaptiveTrackingWndY(double fy)
{
    Configuration::getInstance()->dynamicModelParameters.trackingWndAdaptiveFactorY = fy;

    printf("Dynamic Model :: Update :: Adaptive Tracking Window: Factor Y: %.2f\n", fy);
}

void MainWindow::updateDynamicModel_SetImpactFloorDistTolerance(int tol)
{
    Configuration::getInstance()->dynamicModelParameters.impactMaxFloorDistance = (double)tol;

    printf("Dynamic Model :: Update :: Impact Floor Distance Tolerance: %.2f [mm]\n", Configuration::getInstance()->dynamicModelParameters.impactMaxFloorDistance);
}

void MainWindow::updateDynamicModel_SetFreePlay(bool status)
{
    Configuration::getInstance()->dynamicModelParameters.freePlay = status;
    Configuration::getInstance()->publish();
}

void MainWindow::trackingWindowVisualizeToggled(bool status)
{
    Configuration::getInstance()->dynamicModelParameters.visualizeTrackingWnd = status;
}

void MainWindow::trackingWindowUseToggled(bool status)
{
    if (status) {
        ui->checkBox_tgtPred_VisualizeWnd->setEnabled(true);
        ui->radioButton_tgtPred_AdaptiveTWND->setEnabled(true);
        ui->radioButton_tgtPred_StaticTWND->setEnabled(true);
        ui->label_tgtPred_TrackingWndSize->setEnabled(true);
        ui->slider_tgtPred_TrackingWndSize->setEnabled(true);
        ui->lcd_tgtPredTrackingWndSize->setEnabled(true);
        ui->doubleSpinBox_AdaptiveTrackingWndX->setEnabled(true);
        ui->doubleSpinBox_AdaptiveTrackingWndY->setEnabled(true);
        ui->label_tgtPred_TrackingWndFactor->setEnabled(true);

        updateDynamicModel_UsePredatorTrackingWindow(true);
        updateDynamicModel_VisualizePredatorTrackingWindow(ui->checkBox_tgtPred_VisualizeWnd->isChecked());
        updateDynamicModel_SetTrackingWindowSize(ui->slider_tgtPred_TrackingWndSize->value());

        if (ui->radioButton_tgtPred_AdaptiveTWND->isChecked())
        {
            updateDynamicModel_SetTrackingWindowMode(AdaptiveTrackingWindow);
        }
        else if (ui->radioButton_tgtPred_StaticTWND->isChecked())
        {
            updateDynamicModel_SetTrackingWindowMode(StaticTrackingWindow);
        }

    }
    else {
        updateDynamicModel_UsePredatorTrackingWindow(false);
        updateDynamicModel_VisualizePredatorTrackingWindow(false);
        updateDynamicModel_SetTrackingWindowSize(ui->slider_tgtPred_TrackingWndSize->value());

        ui->label_tgtPred_TrackingWndFactor->setEnabled(false);
        ui->checkBox_tgtPred_VisualizeWnd->setEnabled(false);
        ui->radioButton_tgtPred_AdaptiveTWND->setEnabled(false);
        ui->radioButton_tgtPred_StaticTWND->setEnabled(false);
        ui->label_tgtPred_TrackingWndSize->setEnabled(false);
        ui->slider_tgtPred_TrackingWndSize->setEnabled(false);
        ui->lcd_tgtPredTrackingWndSize->setEnabled(false);
        ui->doubleSpinBox_AdaptiveTrackingWndX->setEnabled(false);
        ui->doubleSpinBox_AdaptiveTrackingWndY->setEnabled(false);
    }
}

void MainWindow::updateDynamicModel_UsePredatorTrackingWindow(bool status)
{
    Configuration::getInstance()->dynamicModelParameters.trackingWndEnabled = status;
}

void MainWindow::updateDynamicModel_VisualizePredatorTrackingWindow(bool status)
{
    Configuration::getInstance()->dynamicModelParameters.visualizeTrackingWnd = status;
}

void MainWindow::updateDynamicModel_SetTrackingWindowMode(TrackingWindowMode tWndMode)
{
    Configuration::getInstance()->dynamicModelParameters.trackingWndMode = tWndMode;
}

void MainWindow::updateDynamicModel_SetTrackingWindowSize(int size)
{
    Configuration::getInstance()->dynamicModelParameters.trackingWndSize = size;
}
