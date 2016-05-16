#ifndef UICALIBRATIONDISPLAY_H
#define UICALIBRATIONDISPLAY_H

#include <QDialog>
#include "../OpticalLayer/HSVManager.h"

namespace Ui {
class UICalibrationDisplay;
}

class UICalibrationDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit UICalibrationDisplay(QWidget *parent = 0);
    ~UICalibrationDisplay();

public slots:
    void receiveTargetHSVRange(HSVRange targetHSVRange);
    void receiveFieldMarkersHSVRange(HSVRange fieldMarkersHSVRange);
    void setGPUMinSegmentLength(int minSegmentLen);
    void setGPUMaxSegmentDistance(int maxSegmentDistance);
    void provideFrame();
    void disableCalibBtnFLD();
    void enableCalibBtnFLD();

private:
    Ui::UICalibrationDisplay *ui;
    HSVRange tgt_hsv_range;
    HSVRange field_markers_hsv_range;
};

#endif // UICALIBRATIONDISPLAY_H
