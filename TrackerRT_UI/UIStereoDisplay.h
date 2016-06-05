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
    UIStereoDisplay(QDialog *parent = 0);
    ~UIStereoDisplay();
    void init(bool stereo, bool autoFetch);
    GLWidget *glWidget;
    GLWidget *glWidgetR;

public slots:
    void fetch();
    void updateRawData();
    void calibrateTarget();
    void calibratePerimeter();
    void calibrateField();

private:
    Ui::UIStereoDisplay *ui;
    bool _stereo;
    bool _autoFetch;
    QGridLayout *mainLayout;

signals:
    void floorLinearModelReady(void);

private slots:
    void renderStereoRawData();
};

#endif // UISTEREODISPLAY_H
