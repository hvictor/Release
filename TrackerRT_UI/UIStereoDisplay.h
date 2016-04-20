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

public slots:
    void fetch();

private:
    Ui::UIStereoDisplay *ui;
    GLWidget *glWidget;
    GLWidget *glWidgetR;
    bool _stereo;
    bool _autoFetch;

private slots:
    void renderStereoRawData();
};

#endif // UISTEREODISPLAY_H
