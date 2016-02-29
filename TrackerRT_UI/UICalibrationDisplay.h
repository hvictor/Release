#ifndef UICALIBRATIONDISPLAY_H
#define UICALIBRATIONDISPLAY_H

#include <QDialog>

namespace Ui {
class UICalibrationDisplay;
}

class UICalibrationDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit UICalibrationDisplay(QWidget *parent = 0);
    ~UICalibrationDisplay();

private:
    Ui::UICalibrationDisplay *ui;
};

#endif // UICALIBRATIONDISPLAY_H
