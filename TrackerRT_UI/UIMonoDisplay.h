#ifndef UISTEREODISPLAY_H
#define UISTEREODISPLAY_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "GLWidget.h"

namespace Ui {
class UIMonoDisplay;
}

class UIMonoDisplay : public QWidget
{
    Q_OBJECT

public:
    UIMonoDisplay();
    ~UIMonoDisplay();

private:
    Ui::UIMonoDisplay *ui;
    GLWidget *glWidget;

public slots:
    void render();

};

#endif // UISTEREODISPLAY_H
