#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <UIModel.h>
#include "UIStereoDisplay.h"
#include "../Configuration/Configuration.h"
#include "../TargetPredator/TargetPredator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UIModel *uiModel;
    UIStereoDisplay *stereoDisplay;

public slots:
    void cpuCoreChanged(int value);
    void startCalibrator();
    void chooseRecordDirectory();
    void updateViewProcessingMode(int processingMode);
    void startApplication();
    void stopApplication();
    void setInputDevice(int dev);
    void loadConfiguration();
    void updatePlaybackParameters_SetPlaybackLatencyDivisor(int value);
    void updateStaticModel_LinesSensitivityEPS(int value);
    void updateStaticModel_SetFloorPlaneLinearModelFactorX(double a);
    void updateStaticModel_SetFloorPlaneLinearModelFactorY(double b);
    void updateStaticModel_SetFloorPlaneLinearModelFactorZ(double c);
    void updateStaticModel_SetFloorPlaneLinearModelFactorD(double d);
    void updateStaticModel_SetNetHeight(int value);
    void updateStaticModel_SetStaticModelMeasureSamples(int value);
    void updateOpticalLayerParam_LowPassFilterY(int value);
    void updateOpticalLayerParam_LowPassFilterX(int value);
    void updateDynamicModel_DFI(int value);
    void toggleDepthMeasurementsStatus(bool status);
    void toggleNotifyTGTLSTStatus(bool status);
    void trackingWindowUseToggled(bool status);
    void trackingWindowVisualizeToggled(bool status);
    void updateDynamicModel_UsePredatorTrackingWindow(bool status);
    void updateDynamicModel_VisualizePredatorTrackingWindow(bool status);
    void updateDynamicModel_SetTrackingWindowMode(TrackingWindowMode tWndMode);
    void updateDynamicModel_SetTrackingWindowSize(int size);
    void updateDynamicModel_SetAdaptiveTrackingWndX(double fx);
    void updateDynamicModel_SetAdaptiveTrackingWndY(double fy);
    void updateDynamicModel_SetTargetLostThreshold(int value);
    void updateDynamicModel_SetVelocityVectorAngleThresh(int value);
    void trackingWindowSetAdaptiveTrackingWindow(bool status);
    void trackingWindowSetStaticTrackingWindow(bool status);
    void updateDynamicModel_SetFreePlay(bool status);
    void updateDynamicModel_Confidence(int value);
    void updateDynamicModel_UseInputKalmanFilter(bool status);
    void updateDynamicModel_SetImpactFloorDistTolerance(int tol);
    void updatePlayModel_SetTwoPlayersLogic(bool status);
    void updatePlayModel_SetSinglePlayerLogic(bool status);
    void saveConfig();
    void showFloorLinearModelCoefficients(void);
    void updateHardwareParam_SetAcquisitionPeriod(double value);
};

#endif // MAINWINDOW_H
