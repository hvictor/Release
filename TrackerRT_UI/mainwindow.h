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
    void updateStaticModel_LinesSensitivityEPS(int value);
    void updateOpticalLayerParam_LowPassFilterY(int value);
    void updateOpticalLayerParam_LowPassFilterX(int value);
    void updateDynamicModel_DFI(int value);
    void toggleDepthMeasurementsStatus(bool status);
    void trackingWindowUseToggled(bool status);
    void updateDynamicModel_UsePredatorTrackingWindow(bool status);
    void updateDynamicModel_VisualizePredatorTrackingWindow(bool status);
    void updateDynamicModel_SetTrackingWindowMode(TrackingWindowMode tWndMode);
    void updateDynamicModel_SetTrackingWindowSize(int size);
    void trackingWindowSetAdaptiveTrackingWindow(bool status);
    void trackingWindowSetStaticTrackingWindow(bool status);
    void saveConfig();
};

#endif // MAINWINDOW_H
