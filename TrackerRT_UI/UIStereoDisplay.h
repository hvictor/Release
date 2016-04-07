#ifndef UISTEREODISPLAY_H
#define UISTEREODISPLAY_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "GLWidget.h"

namespace Ui {
class UIStereoDisplay;
}

class UIStereoDisplay : public QWidget
{
    Q_OBJECT

public:
    UIStereoDisplay();
    ~UIStereoDisplay();

private:
    Ui::UIStereoDisplay *ui;
    GLWidget *glWidget;

private slots:
    void renderStereoRawData();
};

#endif // UISTEREODISPLAY_H
